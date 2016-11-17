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
    /// Interaction logic for ReplayUserControls.xaml
    /// </summary>
    public partial class ReplayUserControls : System.Windows.Controls.UserControl
    {
        ReplayViewModel model = null;
        public ReplayUserControls()
        {
            InitializeComponent();

            List<string> list = new List<string>();
            list.Add(StringHelper.FindLanguageResource("PLATRecord"));
            list.Add(StringHelper.FindLanguageResource("PURecord"));
            RecordType.ItemsSource = list;
            RecordType.SelectedIndex = 0;

            model = new ReplayViewModel(this);
            this.DataContext = model;

            this.StartTime.Text = System.DateTime.Today.ToString("yyyyMMddHHmmss");
            this.EndTime.Text = System.DateTime.Today.AddDays(1).ToString("yyyyMMddHHmmss");
        }

        private void StartReplay_Click(object sender, RoutedEventArgs e)
        {
            model.StartReplayProcess();
        }

        private void StopReplay_Click(object sender, RoutedEventArgs e)
        {
            model.StopReplayProcess();
        }

        private void LocalFileName_GotMouseCapture(object sender, System.Windows.Input.MouseEventArgs e)
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
                    this.LocalFileName.Text = openFileDialog.FileName;
                }));
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
            }
        }
    }
}
