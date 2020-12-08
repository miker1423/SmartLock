using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmartLock.Shared.Models
{
    public class Home
    {
        public User Admin { get; set; }
        public List<Device> Devices { get; set; }
    }
}
