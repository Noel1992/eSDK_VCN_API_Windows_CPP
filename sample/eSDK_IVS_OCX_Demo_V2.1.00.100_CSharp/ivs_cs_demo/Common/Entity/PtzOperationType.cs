using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ivs_cs_demo
{
    public enum PtzOperationType
    {
        PTZ_STOP = 1,
        PTZ_UP,
        PTZ_DOWN,
        PTZ_LEFT,
        PTZ_UP_LEFT,
        PTZ_DOWN_LEFT,
        PTZ_RIGHT,
        PTZ_UP_RIGHT,
        PTZ_DOWN_RIGHT,
        PTZ_AUTO,
        PTZ_PREFAB_BIT_RUN,
        PTZ_CRUISE_RUN,
        PTZ_CRUISE_STOP,
        PTZ_MODE_CRUISE_RUN,
        PTZ_MODE_CRUISE_STOP,
        PTZ_MENU_OPEN,
        PTZ_MENU_EXIT,
        PTZ_MENU_ENTER,
        PTZ_FLIP,
        PTZ_START,
        PTZ_LENS_APERTURE_OPEN,
        PTZ_LENS_APERTURE_CLOSE,
        PTZ_LENS_ZOOM_IN,
        PTZ_LENS_ZOOM_OUT,
        PTZ_LENS_FOCAL_NEAT,
        PTZ_LENS_FOCAL_FAR,
        PTZ_AUX_OPEN,
        PTZ_AUX_STOP,
        MODE_SET_START,
        MODE_SET_STOP,
        PTZ_FAST_LOCATE,
        PTZ_HORIZONTAL_SCAN,
        PTZ_VERTICAL_SCAN,
        PTZ_LOCK,
        PTZ_UNLOCK
    }
}
