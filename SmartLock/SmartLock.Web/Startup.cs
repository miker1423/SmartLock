using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.DependencyInjection;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using SmartLock.Web.Extensions;
using SmartLock.Web.DbContetxs;
using Microsoft.EntityFrameworkCore;

namespace SmartLock.Web
{
    public class Startup
    {
        private IWebHostEnvironment _environment;
        public Startup(IWebHostEnvironment environment)
            => _environment = environment;


        // This method gets called by the runtime. Use this method to add services to the container.
        // For more information on how to configure your application, visit https://go.microsoft.com/fwlink/?LinkID=398940
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddControllers();
            services.AddDbContext<SmartLockDbContext>(options =>
            {
                if (_environment.IsDevelopment())
                    options.UseInMemoryDatabase("db");
                else if (_environment.IsProduction())
                    options.UseSqlServer("");
            });
            services.AddMqttClientHostedService();
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IWebHostEnvironment env)
        {
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }

            app.UseRouting();

            app.UseEndpoints(endpoints => endpoints.MapControllers());
        }
    }
}
