using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace ivs_cs_demo
{
    /// <summary>
    /// Interaction logic for LogLogOutControls.xaml
    /// </summary>
    public partial class LogInLogOutControls : UserControl
    {
        public LogInLogOutViewModel logInLogOutViewModel { get; set; }
        public LogInLogOutControls()
        {
            InitializeComponent();

             logInLogOutViewModel = new LogInLogOutViewModel(this);
             this.DataContext = logInLogOutViewModel;

             List<UserEntity> list = XmlHelper.GetAllUser();
             UserList.ItemsSource = list;
        }

        private void UserLit_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (UserList.SelectedIndex > -1)
            {
                UserEntity user = UserList.SelectedItem as UserEntity;

                this.PortTextBox.Text = user.port;
                this.ServeTextBox.Text = user.ip;
            }
        }
    }
}
