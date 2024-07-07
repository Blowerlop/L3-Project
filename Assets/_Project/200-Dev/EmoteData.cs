using UnityEngine;

namespace Project
{
    [CreateAssetMenu(menuName = "Scriptable Objects/Emote Data")]
    public class EmoteData : ScriptableObject, IScriptableObjectSerializeReference
    {
        public string EmoteName;
        public Sprite EmoteSprite;
    }
}