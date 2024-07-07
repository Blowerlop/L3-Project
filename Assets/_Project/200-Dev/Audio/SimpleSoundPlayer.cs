using _Project._200_Dev.Managers;
using UnityEngine;

namespace _Project._200_Dev.Audio
{
    public class SimpleSoundPlayer : MonoBehaviour
    {
        public string eventId, alias;
        public SoundManager.EventType type;
        public bool playOnStart = true;
        public bool clearOnChange;
        
        void Start()
        {
            if (playOnStart)
                SoundManager.instance.PlayStaticSound(eventId, alias, null, type);
        }

        void Update()
        {
        
        }

        private void OnDestroy()
        {
            if (clearOnChange)
            {
                if (SoundManager.IsInstanceAlive()) SoundManager.instance.StopStaticSound(alias);
            }
        }
    }
}
