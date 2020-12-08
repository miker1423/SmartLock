using Microsoft.Extensions.DependencyInjection;
using System;
using SmartLock.Web.Options;
using MQTTnet;

namespace SmartLock.Web.Extensions
{
    public static class MqttServiceExtension
    {
        public static IServiceCollection AddMqttClientHostedService(this IServiceCollection services)
        {
            services.AddMqttClientServiceWithConfig(config =>
            {
                config.WithTcpServer("192.168.1.246").WithClientId("server");
            });

            return services;
        }

        private static IServiceCollection AddMqttClientServiceWithConfig(this IServiceCollection services, Action<AspCoreMqttClientOptionBuilder> configure)
        {
            services.AddSingleton(serviceProvider =>
            {
                var builder = new AspCoreMqttClientOptionBuilder(serviceProvider);
                configure(builder);
                return builder.Build();
            });

            var factory = new MqttFactory();
            services.AddSingleton(factory.CreateMqttClient());
            services.AddHostedService<MqttWorker>();

            return services;
        }
    }
}
