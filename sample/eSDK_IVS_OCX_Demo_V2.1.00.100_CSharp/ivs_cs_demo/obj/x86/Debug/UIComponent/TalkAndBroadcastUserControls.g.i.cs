﻿#pragma checksum "..\..\..\..\UIComponent\TalkAndBroadcastUserControls.xaml" "{406ea660-64cf-4c82-b6f0-42d48172a799}" "3395149AF0139169C34B77EEFED0CB94"
//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:4.0.30319.42000
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Forms.Integration;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Effects;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Media.TextFormatting;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Shell;


namespace ivs_cs_demo {
    
    
    /// <summary>
    /// TalkAndBroadcastUserControls
    /// </summary>
    public partial class TalkAndBroadcastUserControls : System.Windows.Controls.UserControl, System.Windows.Markup.IComponentConnector {
        
        
        #line 34 "..\..\..\..\UIComponent\TalkAndBroadcastUserControls.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.ComboBox ProtocolType;
        
        #line default
        #line hidden
        
        
        #line 35 "..\..\..\..\UIComponent\TalkAndBroadcastUserControls.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.CheckBox DirectFirst;
        
        #line default
        #line hidden
        
        
        #line 37 "..\..\..\..\UIComponent\TalkAndBroadcastUserControls.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.TextBox TalkCameraCode;
        
        #line default
        #line hidden
        
        
        #line 40 "..\..\..\..\UIComponent\TalkAndBroadcastUserControls.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.TextBox TalkbackHandle;
        
        #line default
        #line hidden
        
        
        #line 64 "..\..\..\..\UIComponent\TalkAndBroadcastUserControls.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.TextBox BroadcastDeviceCode;
        
        #line default
        #line hidden
        
        
        #line 71 "..\..\..\..\UIComponent\TalkAndBroadcastUserControls.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.TextBox FileName;
        
        #line default
        #line hidden
        
        
        #line 75 "..\..\..\..\UIComponent\TalkAndBroadcastUserControls.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.ComboBox CycleType;
        
        #line default
        #line hidden
        
        
        #line 78 "..\..\..\..\UIComponent\TalkAndBroadcastUserControls.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.ListBox BroadcastDeviceList;
        
        #line default
        #line hidden
        
        
        #line 104 "..\..\..\..\UIComponent\TalkAndBroadcastUserControls.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.TextBox eSDKCodeTextBlock;
        
        #line default
        #line hidden
        
        private bool _contentLoaded;
        
        /// <summary>
        /// InitializeComponent
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.CodeDom.Compiler.GeneratedCodeAttribute("PresentationBuildTasks", "4.0.0.0")]
        public void InitializeComponent() {
            if (_contentLoaded) {
                return;
            }
            _contentLoaded = true;
            System.Uri resourceLocater = new System.Uri("/ivs_cs_demo;component/uicomponent/talkandbroadcastusercontrols.xaml", System.UriKind.Relative);
            
            #line 1 "..\..\..\..\UIComponent\TalkAndBroadcastUserControls.xaml"
            System.Windows.Application.LoadComponent(this, resourceLocater);
            
            #line default
            #line hidden
        }
        
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.CodeDom.Compiler.GeneratedCodeAttribute("PresentationBuildTasks", "4.0.0.0")]
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Never)]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Design", "CA1033:InterfaceMethodsShouldBeCallableByChildTypes")]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Maintainability", "CA1502:AvoidExcessiveComplexity")]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1800:DoNotCastUnnecessarily")]
        void System.Windows.Markup.IComponentConnector.Connect(int connectionId, object target) {
            switch (connectionId)
            {
            case 1:
            this.ProtocolType = ((System.Windows.Controls.ComboBox)(target));
            return;
            case 2:
            this.DirectFirst = ((System.Windows.Controls.CheckBox)(target));
            return;
            case 3:
            this.TalkCameraCode = ((System.Windows.Controls.TextBox)(target));
            return;
            case 4:
            this.TalkbackHandle = ((System.Windows.Controls.TextBox)(target));
            return;
            case 5:
            this.BroadcastDeviceCode = ((System.Windows.Controls.TextBox)(target));
            return;
            case 6:
            this.FileName = ((System.Windows.Controls.TextBox)(target));
            
            #line 71 "..\..\..\..\UIComponent\TalkAndBroadcastUserControls.xaml"
            this.FileName.GotMouseCapture += new System.Windows.Input.MouseEventHandler(this.FileName_GotMouseCapture);
            
            #line default
            #line hidden
            return;
            case 7:
            this.CycleType = ((System.Windows.Controls.ComboBox)(target));
            return;
            case 8:
            this.BroadcastDeviceList = ((System.Windows.Controls.ListBox)(target));
            return;
            case 9:
            this.eSDKCodeTextBlock = ((System.Windows.Controls.TextBox)(target));
            return;
            }
            this._contentLoaded = true;
        }
    }
}

