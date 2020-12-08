using Newtonsoft.Json;
using SmartLock.Mobile.DataModels;
using SmartLock.Mobile.Models;
using System;
using System.Diagnostics;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace SmartLock.Mobile.ViewModels
{
    [QueryProperty(nameof(ItemId), nameof(ItemId))]
    public class ItemDetailViewModel : BaseViewModel
    {
        private string itemId;
        private string text;
        private string description;
        private readonly HttpClient _httpClient = new HttpClient()
        {
            BaseAddress = new Uri("http://192.168.1.241:5000")
        };
        public string Id { get; set; }
        public Command LinkDeviceCommand { get; }

        public ItemDetailViewModel()
            => LinkDeviceCommand = new Command(LinkDevice);

        public string Text
        {
            get => text;
            set => SetProperty(ref text, value);
        }

        public string Description
        {
            get => description;
            set => SetProperty(ref description, value);
        }

        public string ItemId
        {
            get
            {
                return itemId;
            }
            set
            {
                itemId = value;
                LoadItemId(value);
            }
        }

        public async void LinkDevice()
        {
            var item = new DeviceRegisterVM()
            {
                DeviceMacAddress = Description,
                Username = "Miguel"
            };
            var serialized = JsonConvert.SerializeObject(item);
            var httpContent = new StringContent(serialized, Encoding.UTF8, "application/json");
            var result = await _httpClient.PostAsync("/devices", httpContent);
            if (result.IsSuccessStatusCode) return;
            else Debug.WriteLine("Failed");
        }

        public async void LoadItemId(string itemId)
        {
            try
            {
                var item = await DataStore.GetItemAsync(itemId);
                Id = item.Id;
                Text = item.Text;
                Description = item.Description;
            }
            catch (Exception)
            {
                Debug.WriteLine("Failed to Load Item");
            }
        }
    }
}
