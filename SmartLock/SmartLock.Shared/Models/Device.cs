using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace SmartLock.Shared.Models
{
    public class Device
    {
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        [Key]
        public Guid DeviceId { get; set; }
        public User DeviceOwner { get; set; }
        public string MacAddress { get; set; }
        public List<TagRegistry> Registries { get; set; }
    }
}
