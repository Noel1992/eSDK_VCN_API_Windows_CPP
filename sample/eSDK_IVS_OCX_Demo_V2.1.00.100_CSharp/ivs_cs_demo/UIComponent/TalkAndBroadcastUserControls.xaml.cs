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
using System.Windows.Forms;

namespace ivs_cs_demo
{
    /// <summary>
    /// Interaction logic for TalkAndBroadcastUserControls.xaml
    /// </summary>
    public partial class TalkAndBroadcastUserControls : System.Windows.Controls.UserControl
    {
        TalkAndBroadcastViewModel model = null;
        public TalkAndBroadcastUserControls()
        {
            InitializeComponent();

            List<string> listCycle = new List<string>(); 
            listCycle.Add(StringHelper.FindLanguageResource("CycleFalse"));
            listCycle.Add(StringHelper.FindLanguageResource("CycleTrue"));

            CycleType.ItemsSource = listCycle;
            CycleType.SelectedIndex = 0;

            model = new TalkAndBroadcastViewModel(this);
            this.DataContext = model;

            List<string> listProtocol = new List<string>(); 
            listProtocol.Add("UDP");
            listProtocol.Add("TCP");

            ProtocolType.ItemsSource = listProtocol;
            ProtocolType.SelectedIndex = 0;
        }

        private void FileName_GotMouseCapture(object sender, System.Windows.Input.MouseEventArgs e)
        {
            try
            {
                System.Windows.Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                {
                    OpenFileDialog openFileDialog = new OpenFileDialog();
                    openFileDialog.Title = StringHelper.FindLanguageResource("SelectFile");
                    openFileDialog.FileName = string.Empty;
                    openFileDialog.FilterIndex = 1;
                    openFileDialog.RestoreDirectory = true;
                    DialogResult result = openFileDialog.ShowDialog();
                    if (result == System.Windows.Forms.DialogResult.Cancel)
                    {
                        return;
                    }
                    this.FileName.Text = openFileDialog.FileName;
                }));
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
            }
        }
    }
}
