using Project._Project.Scripts.Player.States;
using Sirenix.OdinInspector;
using Unity.Netcode;
using UnityEngine;

namespace Project
{
    public class EmoteController : NetworkBehaviour
    {
        [SerializeField] private PCPlayerRefs refs;

        [RequiredListLength(4)]
        [SerializeField] private EmoteData[] emotes;
        
        [Server]
        public void TryPlayEmote(int emoteIndex)
        {
            var state = new EmoteState(emoteIndex);
            if (refs.StateMachine.CanChangeStateTo<EmoteState>())
            {
                refs.StateMachine.ChangeStateTo(state);
            }
        }
        
        [ServerRpc(RequireOwnership = false)]
        public void TryPlayEmoteServerRpc(int emoteIndex)
        {
            TryPlayEmote(emoteIndex);
        }

        public EmoteData GetEmote(int index) => emotes[index];
    }
}