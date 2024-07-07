using Project._Project.Scripts.Managers;
using UnityEngine;

namespace Project
{
    public class SoundAnimEventHandler : MonoBehaviour
    {
        public void PlayStaticSound(AnimationEvent eventId)
        {
            SoundManager.instance.PlayStaticSound(eventId.stringParameter,
                gameObject.name + eventId.intParameter, gameObject,
                SoundManager.EventType.Spell);
        }

        public void TriggerSustain(int eventID)
        {
            SoundManager.instance.TriggerSustain(gameObject.name + eventID);
        }
    }
}
