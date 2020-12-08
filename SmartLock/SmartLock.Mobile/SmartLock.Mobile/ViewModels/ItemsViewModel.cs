using SmartLock.Mobile.Models;
using SmartLock.Mobile.Views;
using System;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Threading.Tasks;
using Xamarin.Forms;
using System.Net.Sockets;
using System.Text;
using System.Net;

namespace SmartLock.Mobile.ViewModels
{
    public class ItemsViewModel : BaseViewModel
    {
        private Item _selectedItem;

        public ObservableCollection<Item> Items { get; }
        public Command LoadItemsCommand { get; }
        public Command AddItemCommand { get; }
        public Command<Item> ItemTapped { get; }

        public ItemsViewModel()
        {
            Title = "Browse";
            Items = new ObservableCollection<Item>();
            LoadItemsCommand = new Command(async () => await ExecuteLoadItemsCommand());

            ItemTapped = new Command<Item>(OnItemSelected);

            AddItemCommand = new Command(OnAddItem);
        }

        async Task ExecuteLoadItemsCommand()
        {
            IsBusy = true;

            try
            {
                Items.Clear();
                var client = new UdpClient()
                {
                    EnableBroadcast = true,
                    DontFragment = true
                };
                var data = await DataStore.GetItemsAsync();
                foreach (var item in data)
                    Items.Add(item);
                var buffer = new byte[] { (byte)'d' };
                _ = Task.Run(async () =>
                {
                    while (true)
                    {
                        var result = await client.ReceiveAsync();
                        var str = Encoding.ASCII.GetString(result.Buffer);
                        var item = new Item
                        {
                            Description = str,
                            Id = str,
                            Text = "Smart Lock"
                        };
                        var exists = await DataStore.GetItemAsync(item.Id);
                        if(exists is null)
                        {
                            await DataStore.AddItemAsync(item);
                            Items.Add(item);
                        }
                    }
                });

                await client.SendAsync(buffer, buffer.Length, "192.168.1.255", 1002);
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex);
            }
            finally
            {
                IsBusy = false;
            }
        }

        public void OnAppearing()
        {
            IsBusy = true;
            SelectedItem = null;
        }

        public Item SelectedItem
        {
            get => _selectedItem;
            set
            {
                SetProperty(ref _selectedItem, value);
                OnItemSelected(value);
            }
        }

        private async void OnAddItem(object obj)
        {
            await Shell.Current.GoToAsync(nameof(NewItemPage));
        }

        async void OnItemSelected(Item item)
        {
            if (item == null)
                return;

            // This will push the ItemDetailPage onto the navigation stack
            await Shell.Current.GoToAsync($"{nameof(ItemDetailPage)}?{nameof(ItemDetailViewModel.ItemId)}={item.Id}");
        }
    }
}