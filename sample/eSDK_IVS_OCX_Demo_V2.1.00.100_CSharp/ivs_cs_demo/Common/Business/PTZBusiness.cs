using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ivs_cs_demo
{
    public class PTZBusiness
    {
        private PtzService ptzService = new PtzService();

        public int SendPtz(PtzOperationType operation, int param1, int param2, string code)
        {
            return ptzService.SendPtz(operation, param1, param2, code);
        }
    }
}
