using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ivs_cs_demo
{
    [Flags]
    public enum LivePlayerToolType
    {
        Snapshot = 1 << 0,
        LocalRecord = 1 << 1,
        Bookmark = 1 << 2,
        Zoom = 1 << 3,
        PlayRecord = 1 << 4,
        PlaySound = 1 << 5,
        Talkback = 1 << 6,
        VideoTvw = 1 << 7,
        AlarmWin = 1 << 8,
        PTZ = 1 << 9,
        MA = 1 << 10,
        OpenMap = 1 << 11,
        WindowMain = 1 << 12,
        PlayQuality = 1 << 13,
        All = Snapshot | LocalRecord | Bookmark | Zoom | PlayRecord | PlaySound | Talkback | VideoTvw | AlarmWin | PTZ | MA | OpenMap | WindowMain | PlayQuality
    }
}
