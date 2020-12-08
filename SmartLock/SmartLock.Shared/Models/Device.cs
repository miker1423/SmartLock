using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmartLock.Shared.Models
{
    public class Device
    {
        public Home DeviceLocation { get; set; }
        public List<TagRegistry> Registries { get; set; }
    }
}
