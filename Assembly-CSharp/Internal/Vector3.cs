using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;

namespace Internal
{
    class Vector3
    {
        public Vector3() { }
        public Vector3(float X, float Y, float Z) { m_X = X; m_Y = Y; m_Z = Z; }
        public Vector3(float VectorValue) { m_X = VectorValue; m_Y = VectorValue; m_Z = VectorValue; }

        public override string ToString() { return "(" + m_X + ", " + m_Y + ", " + m_Z + ")"; }

        protected float m_X;
        protected float m_Y;
        protected float m_Z;
        
        public float X { get { return m_X; } set { m_X = value; } }
        public float Y { get { return m_Y; } set { m_Y = value; } }
        public float Z { get { return m_Z; } set { m_Z = value; } }

        public static Vector3 Up { get { return new Vector3(0.0f, 1.0f, 0.0f); } }
        public static Vector3 Down { get { return new Vector3(0.0f, -1.0f, 0.0f); } }
        public static Vector3 Left { get { return new Vector3(-1.0f, 0.0f, 0.0f); } }
        public static Vector3 Right { get { return new Vector3(1.0f, 0.0f, 0.0f); } }
        public static Vector3 Forwad { get { return new Vector3(0.0f, 0.0f, 1.0f); } }
        public static Vector3 Backward { get { return new Vector3(0.0f, 0.0f, -1.0f); } }
        public static Vector3 Zero { get { return new Vector3(0.0f, 0.0f, 0.0f); } }
        public static Vector3 One { get { return new Vector3(1.0f, 1.0f, 1.0f); } }

        /// <summary>
        /// Add two vectors and modify this
        /// </summary>
        /// <param name="rhs">Vector to add to this</param>
        public void Plus(Vector3 rhs)
        {
            m_X += rhs.X;
            m_Y += rhs.Y;
            m_Z += rhs.Z;
        }

        /// <summary>
        ///  Minus two vectors and modify this
        /// </summary>
        /// <param name="rhs">Vector to minus from this</param>
        public void Minus(Vector3 rhs)
        {
            m_X -= rhs.X;
            m_Y -= rhs.Y;
            m_Z -= rhs.Z;
        }

        /// <summary>
        /// Get the length squared of this vector
        /// </summary>
        /// <returns>Vector length squared</returns>
        public double LengthSquared()
        {
            return Math.Sqrt(Math.Pow(m_X, 2) + Math.Pow(m_Y, 2) + Math.Pow(m_Z, 2));
        }

        /// <summary>
        /// Returns a vector whos direction is perpandicular to this and another vector
        /// </summary>
        /// <param name="rhs">Vector to compare against</param>
        /// <returns>Perpandicular vector</returns>
        public Vector3 Cross(Vector3 rhs)
        {
            //  i  j  k
            // x1 y1 z1
            // x2 y2 z2

            float i = (m_Y * rhs.Z) - (m_Z * rhs.Y);
            float j = (m_X * rhs.Z) - (m_Z * rhs.X);
            float k = (m_X * rhs.Y) - (m_Y * rhs.X);

            return new Vector3(i, j, k);
        }

        /// <summary>
        /// Returns the dot product between two vectors
        /// </summary>
        /// <param name="rhs">Vector to compare against</param>
        /// <returns>Angle</returns>
        public float Dot(Vector3 rhs)
        {
            float Angle = 0.0f;

            float a = m_X * rhs.m_X;
            float b = m_Y * rhs.m_Y;
            float c = m_Z * rhs.m_Z;
            Angle = (a + b + c);

            return Angle;
        }

        /// <summary>
        /// Get the angle in degrees between this and another vector
        /// </summary>
        /// <param name="rhs">Vector whos angle to measure against</param>
        /// <returns>Angle between the two vectors</returns>
        public float AngleBetweenwoVectors(Vector3 rhs)
        {
            return (float)Math.Cos(Dot(rhs));
        }

        /// <summary>
        /// Get the normal of this vector whos magnitude is exactly one
        /// </summary>
        /// <returns>Normalized vector of this</returns>
        public Vector3 Normalize()
        {
            Vector3 normal = Vector3.Zero;
            double length = LengthSquared();
            normal.m_X = (float)(1.0 / length);
            normal.m_Z = (float)(1.0 / length);
            normal.m_Y = (float)(1.0 / length);
            return normal;
        }

    }
}
