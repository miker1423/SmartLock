using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using SmartLock.Shared.ViewModels;
using SmartLock.Web.DbContetxs;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using SmartLock.Shared.Models;
using Microsoft.EntityFrameworkCore.ChangeTracking;

namespace SmartLock.Web.Controllers
{
    [Route("[controller]")]
    public class DevicesController : ControllerBase
    {
        private readonly SmartLockDbContext _context;
        private readonly ILogger<DevicesController> _logger;
        public DevicesController(SmartLockDbContext context,
                                 ILogger<DevicesController> logger)
        {
            _context = context;
            _logger = logger;
        }

        [HttpPost]
        public async Task<IActionResult> Post([FromBody]DeviceRegisterVM deviceRegisterVM)
        {
            var isRegistered = _context.Devices.Any(device => device.MacAddress == deviceRegisterVM.DeviceMacAddress);
            if (isRegistered) return BadRequest();

            var userExists = _context.Users.Any(usr => usr.Username == deviceRegisterVM.Username);
            User user = null;
            if (!userExists) user = _context.Users.Add(new User()
            {
                Username = deviceRegisterVM.Username
            }).Entity;
            else user = _context.Users.First(usr => usr.Username == deviceRegisterVM.Username);

            var device = _context.Devices.Add(new Device
            {
                MacAddress = deviceRegisterVM.DeviceMacAddress,
                Registries = new List<TagRegistry>()
            });
            if (user.Devices is null) user.Devices = new List<Device>();
            user.Devices.Add(device.Entity);

            await _context.SaveChangesAsync();
            _logger.LogInformation("Registered new device");
            return Ok();
        }
    }
}
