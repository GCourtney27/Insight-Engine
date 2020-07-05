using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Navigation;

namespace Internal
{
    class Transform
    {
        public Transform()
        {
            m_Position = new Vector3();
            m_Rotation = new Vector3();
            m_Scale = new Vector3();
        }
        Vector3 m_Position;
        Vector3 m_Rotation;
        Vector3 m_Scale;

        public Vector3 Position { get { return m_Position; } set { m_Position = value; } }
        public Vector3 Rotation { get { return m_Rotation; } set { m_Rotation = value; } }
        public Vector3 Scale { get { return m_Scale; } set { m_Scale = value; } }
        public override string ToString()
        {
            return "Position: " + m_Position.ToString() + ", Rotation: " + m_Rotation.ToString() + "Scale: " + m_Scale.ToString();
        }

        /// <summary>
        /// Set Actor position based on x y z overrides
        /// </summary>
        /// <param name="X">X offset in degrees</param>
        /// <param name="Y">Y offset in degrees</param>
        /// <param name="Z">Z offset in degrees</param>
        public void SetActorPosition(float X, float Y, float Z)
        {
            m_Position.X = X;
            m_Position.Y = Y;
            m_Position.Z = Z;
        }

        /// <summary>
        /// Smooth translate an actor based on a vector offset
        /// </summary>
        /// <param name="Offset">Position offset</param>
        public void TranslateActor(Vector3 Offset)
        {
            m_Position.Plus(Offset);
        }
        
        /// <summary>
        /// Smooth translate an actor based on an x y z offset
        /// </summary>
        /// <param name="X">Offset</param>
        /// <param name="Y">Offset</param>
        /// <param name="Z">Offset</param>
        public void TranslateActor(float X, float Y, float Z)
        {
            m_Position.Plus(new Vector3(X, Y, Z));
        }

        /// <summary>
        /// Smooth rotate an actor based on an x y z offset
        /// </summary>
        /// <param name="Offset">Vector offset</param>
        public void RotateActorPitchYawRoll(Vector3 Offset)
        {
            m_Rotation.Plus(Offset);
        }

        /// <summary>
        /// Smooth rotate an actor based on an x y z offset
        /// </summary>
        /// <param name="X">Offset</param>
        /// <param name="Y">Offset</param>
        /// <param name="Z">Offset</param>
        public void RotateActorPitchYawRoll(float Pitch, float Yaw, float Roll)
        {
            m_Rotation.Plus(new Vector3(Pitch, Yaw, Roll));
        }

        /// <summary>
        /// Set actor scale based on a vector input
        /// </summary>
        /// <param name="Offset">Vector scale override</param>
        public void ScaleActor(Vector3 Offset)
        {
            m_Scale.Plus(Offset);
        }

        /// <summary>
        /// Smooth scale an actor based on x y z offsets
        /// </summary>
        /// <param name="X">Offset</param>
        /// <param name="Y">Offset</param>
        /// <param name="Z">Offset</param>
        public void ScaleActor(float X, float Y, float Z)
        {
            m_Scale.Plus(new Vector3(X, Y, Z));
        }

        /// <summary>
        /// Set actor scale based on x y z overrides
        /// </summary>
        /// <param name="X">Override</param>
        /// <param name="Y">Override</param>
        /// <param name="Z">Override</param>
        public void SetActorScale(float X, float Y, float Z)
        {
            m_Scale.X = X;
            m_Scale.Y = Y;
            m_Scale.Z = Z;
        }

        /// <summary>
        /// Set actor rotation based on x y z overrides
        /// </summary>
        /// <param name="X">Override</param>
        /// <param name="Y">Override</param>
        /// <param name="Z">Override</param>
        public void SetActorRotation(float X, float Y, float Z)
        {
            m_Rotation.X = X;
            m_Rotation.Y = Y;
            m_Rotation.Z = Z;
        }
    }
}
