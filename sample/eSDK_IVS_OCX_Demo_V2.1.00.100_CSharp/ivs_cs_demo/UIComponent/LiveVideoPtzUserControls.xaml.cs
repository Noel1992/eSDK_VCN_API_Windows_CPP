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
    /// Interaction logic for LiveVideoPtzUserControls.xaml
    /// </summary>
    public partial class LiveVideoPtzUserControls : UserControl
    {
        public LiveVideoPtzViewModel liveVideoPtzViewModel { get; set; }

        public LiveVideoPtzUserControls()
        {
            InitializeComponent();
            liveVideoPtzViewModel = new LiveVideoPtzViewModel(this);
            this.DataContext = liveVideoPtzViewModel;
            this.PlayParameter.Text = @"<?xml version='1.0' encoding='utf-8'?>
<Content>
      <RealplayParam>
      <StreamType>1</StreamType>
      <ProtocolType>2</ProtocolType>
      <DirectFirst>0</DirectFirst>
      <Multicast>0</Multicast>
      </RealplayParam>
</Content>";
        }
        private void StopPtzCommand(object sender, MouseButtonEventArgs e)
        {
            liveVideoPtzViewModel.PtzProcess(PtzOperationType.PTZ_STOP);
        }
        private void PtzCommand(object sender, MouseButtonEventArgs e)
        {
            switch ((sender as Button).CommandParameter.ToString())
            {
                case "LeftUP":
                    liveVideoPtzViewModel.PtzProcess(PtzOperationType.PTZ_UP_LEFT);
                    break;
                case "Left":
                    liveVideoPtzViewModel.PtzProcess(PtzOperationType.PTZ_LEFT);
                    break;
                case "UP":
                    liveVideoPtzViewModel.PtzProcess(PtzOperationType.PTZ_UP);
                    break;
                case "LeftDown":
                    liveVideoPtzViewModel.PtzProcess(PtzOperationType.PTZ_DOWN_LEFT);
                    break;
                case "Right":
                    liveVideoPtzViewModel.PtzProcess(PtzOperationType.PTZ_RIGHT);
                    break;
                case "RightUP":
                    liveVideoPtzViewModel.PtzProcess(PtzOperationType.PTZ_UP_RIGHT);
                    break;
                case "RightDown":
                    liveVideoPtzViewModel.PtzProcess(PtzOperationType.PTZ_DOWN_RIGHT);
                    break;
                case "Down":
                    liveVideoPtzViewModel.PtzProcess(PtzOperationType.PTZ_DOWN);
                    break;
                case "Lock":
                    liveVideoPtzViewModel.PtzProcess(PtzOperationType.PTZ_LOCK);
                    break;

                case "UnLock":
                    liveVideoPtzViewModel.PtzProcess(PtzOperationType.PTZ_UNLOCK);
                    break;
                case "ZoomOut":
                    liveVideoPtzViewModel.PtzProcess(PtzOperationType.PTZ_LENS_ZOOM_IN);
                    break;
                case "ZoomIn":
                    liveVideoPtzViewModel.PtzProcess(PtzOperationType.PTZ_LENS_ZOOM_OUT);
                    break;
            }
        }
    }
}
