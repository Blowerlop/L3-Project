using _Project._200_Dev.Tools;
using Sirenix.OdinInspector;
using UnityEngine;

namespace _Project._200_Dev.Game
{
    [CreateAssetMenu(menuName = "Scriptable Objects/Game Settings")]
    public class SOGameSettings : ScriptableObject, IScriptableObjectSerializeReference
    {
        [BoxGroup(GroupName = "Player")] 
        public float deathTime = 10.0f;
    }
}
