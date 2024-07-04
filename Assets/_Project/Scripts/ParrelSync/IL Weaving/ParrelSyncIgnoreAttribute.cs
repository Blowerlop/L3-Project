using System;
using System.Diagnostics;

namespace Project
{
    [Conditional("UNITY_EDITOR")]
    [AttributeUsage(AttributeTargets.Method)]
    public class ParrelSyncIgnoreAttribute : Attribute
    {
    }
}
