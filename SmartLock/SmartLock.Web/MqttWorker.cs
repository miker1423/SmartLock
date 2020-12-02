using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Text;
using MQTTnet;
using MQTTnet.Client.Options;
using MQTTnet.Client;
using MQTTnet.Client.Connecting;
using MQTTnet.Client.Disconnecting;
using MQTTnet.Client.Receiving;

namespace SmartLock.Web
{
    public class MqttWorker : IHostedService, 
                              IMqttApplicationMessageReceivedHandler,
                              IMqttClientConnectedHandler,
                              IMqttClientDisconnectedHandler
    {
        private readonly ILogger<MqttWorker> _logger;
        private readonly IMqttClient _client;
        private readonly IMqttClientOptions _options;

        public MqttWorker(ILogger<MqttWorker> logger, 
                          IMqttClient client,
                          IMqttClientOptions options)
        {
            _logger = logger;
            _client = client;
            _options = options;
            _client.ApplicationMessageReceivedHandler = this;
            _client.ConnectedHandler = this;
            _client.DisconnectedHandler = this;
        }

        public Task HandleApplicationMessageReceivedAsync(MqttApplicationMessageReceivedEventArgs args)
        {
            var message = Encoding.ASCII.GetString(args.ApplicationMessage.Payload);
            _logger.LogInformation("Received message {0}", message);

            return Task.CompletedTask;
        }

        public async Task HandleConnectedAsync(MqttClientConnectedEventArgs args)
        {
            _logger.LogInformation("Client connected");
            var subscribeResult = await _client.SubscribeAsync("devices/#");
            foreach (var result in subscribeResult.Items)
                _logger.LogInformation("Subscribed to {0} with QoS {1}", result.TopicFilter.Topic, result.ResultCode);
        }

        public async Task HandleDisconnectedAsync(MqttClientDisconnectedEventArgs eventArgs)
        {
            await _client.ReconnectAsync();
            while (!_client.IsConnected)
            {
                await _client.ReconnectAsync();
                await Task.Delay(TimeSpan.FromSeconds(1));
            }
        }

        public async Task StartAsync(CancellationToken cancellationToken)
        {
            var result = await _client.ConnectAsync(_options, cancellationToken);
            _logger.LogInformation("Client connected with result code {0}", result.ResultCode);
        }

        public async Task StopAsync(CancellationToken cancellationToken)
        {
            _logger.LogInformation("Client disconnecting...");
            if (_client.IsConnected)
                await _client.DisconnectAsync();

            _logger.LogInformation("Client disconnected");
        }
    }
}
