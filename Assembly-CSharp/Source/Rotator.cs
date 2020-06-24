using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Internal;

namespace InsightEngine
{
    class Rotator : ScriptableBehavior
    {
        // Called when game started or when spawned
        void BeginPlay()
        {

        }

        // Called once per frame
        void Tick(double deltaMs)
        {
            //if (Input.IsMouseButtonPressed(Input.eMouseButton.IE_MOUSEBUTTON_LEFT))
            {
                this.Transform.RotateActorPitchYawRoll(0.0f, (float)Math.Sin(1.0f * deltaMs), 0.0f);
            }
        }
    }
}
