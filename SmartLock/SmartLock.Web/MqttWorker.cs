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
using SmartLock.Web.DbContetxs;
using Microsoft.Extensions.DependencyInjection;

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
        private IServiceScopeFactory _serviceFactory;

        public MqttWorker(ILogger<MqttWorker> logger, 
                          IMqttClient client,
                          IMqttClientOptions options,
                          IServiceScopeFactory serviceFactory)
        {
            _logger = logger;
            _client = client;
            _options = options;
            _client.ApplicationMessageReceivedHandler = this;
            _client.ConnectedHandler = this;
            _client.DisconnectedHandler = this;

            _serviceFactory = serviceFactory;
        }

        public async Task HandleApplicationMessageReceivedAsync(MqttApplicationMessageReceivedEventArgs args)
        {
            var message = Encoding.ASCII.GetString(args.ApplicationMessage.Payload);
            _logger.LogInformation("Received message {0}", message);

            if (args.ApplicationMessage.Topic.Contains("r")) return;

            using var scope = _serviceFactory.CreateScope();
            var dbContext = scope.ServiceProvider.GetRequiredService<SmartLockDbContext>();
            var macAddress = GetMacAddress(args.ApplicationMessage.Topic);
            var result = dbContext.Devices.SingleOrDefault(device => device.MacAddress == macAddress);
            var responseTopic = args.ApplicationMessage.Topic + "/r00";
            if (result is null)
            {
                _logger.LogInformation("Device not registered");
                await _client.PublishAsync(responseTopic, "2,f");
                return;
            }

            var (messageType, thumbprint) = ReadRequest(message);

            if(messageType == MessageType.Auth)
            {

                var registered = dbContext.Entry(result)
                                      .Collection(result => result.Registries)
                                      .Query()
                                      .Any(registry => registry.Thumbprint == thumbprint);
                var regResult = registered ? "t" : "f";
                var response = $"2,{regResult}";
                await _client.PublishAsync(responseTopic, response);
                return;
            } 
            else if(messageType == MessageType.Register)
            {
                var registered = dbContext.TagRegistries.FirstOrDefault(registry => registry.Thumbprint == thumbprint);
                if(registered is null || registered.OwnerDevice.DeviceId != result.DeviceId)
                    dbContext.TagRegistries.Add(new Shared.Models.TagRegistry()
                    {
                        OwnerDevice = result,
                        IsActive = true,
                        Thumbprint = thumbprint
                    }); 
                else
                    _logger.LogInformation("Already registered");
                await dbContext.SaveChangesAsync();
                await _client.PublishAsync(responseTopic, "2,t");
                return;
            }

            await _client.PublishAsync(responseTopic, "2,f");
        }

        private static string GetMacAddress(string topic)
        {
            var firstIndex = topic.IndexOf('/') + 1;
            return topic[firstIndex..];
        }

        private static (MessageType messageType, string thumbprint) ReadRequest(string payload)
        {
            var type = int.Parse(payload[0].ToString());
            var thumbprint = payload[2..];
            return ((MessageType)type, thumbprint);
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

    public enum MessageType
    {
        Auth,
        Register,
        Response
    }
}
