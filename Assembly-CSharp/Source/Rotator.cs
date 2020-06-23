using Internal;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InsightEngine
{
    class Rotator : ScriptableBehavior
    {
        public float Frequency = 1.0f;
        public float Amplitude = 0.5f;

        Vector3 posOffset = new Vector3();
        Vector3 tempPos = new Vector3();
        // Called when game started or when spawned
        void BeginPlay()
        {
            posOffset = this.Transform.Position;
        }

        // Called once per frame
        void Tick(double deltaMs)
        {
            tempPos = posOffset;
            tempPos.Y += (float)(Math.Sin(deltaMs * Math.PI * Frequency) * Amplitude);

            this.Transform.RotateActorPitchYawRoll(0.0f, (float)Math.Sin(1.0f * deltaMs), 0.0f);
            this.Transform.Position = tempPos;
        }
    }
}
