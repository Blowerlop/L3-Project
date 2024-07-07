using Sirenix.OdinInspector;
using UnityEngine;

namespace Project
{
    [CreateAssetMenu(menuName = "Scriptable Objects/Game Settings")]
    public class SOGameSettings : ScriptableObject, IScriptableObjectSerializeReference
    {
        [BoxGroup(GroupName = "Player")] 
        public float deathTime = 10.0f;
    }
}
