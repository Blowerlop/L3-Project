using System;
using _Project._200_Dev.Entities.Player;
using _Project._200_Dev.Entities.Player.States;
using _Project._200_Dev.Networking.IL_Weaving.Server;
using _Project._200_Dev.Utilities;
using Unity.Netcode;
using UnityEngine;

namespace _Project._200_Dev.Spells
{
    public class ChannelingController : NetworkBehaviour
    { 
        [SerializeField] private PlayerRefs playerRefs;
        
        private NetworkVariable<bool> _isChanneling = new();
        
        private readonly Timer _channelingTimer = new Timer();
        
        public event Action OnServerChannelingStarted; 
        public event Action OnServerChannelingEnded;

        public bool IsChanneling => _isChanneling.Value;

        public override void OnNetworkSpawn()
        {
            base.OnNetworkSpawn();

            if (!IsClient) return;

            _isChanneling.OnValueChanged += (_, newValue) =>
            {
                 if (newValue) OnServerChannelingStarted?.Invoke();
                 else OnServerChannelingEnded?.Invoke();
            };
        }
        
        [Server]
        public void StartServerChanneling(float channelingTime, byte index, Action channelingDoneAction = null)
        {
            // Bypass entering channeling state and exiting it instantly if we have no channeling time
            if (channelingTime == 0)
            {
                channelingDoneAction?.Invoke();
                return;
            }
            
            ChangeState(new ChannelingState((byte)(index + 1)));
            
            _isChanneling.Value = true;
            _channelingTimer.StartTimerWithCallback(this, channelingTime, () =>
            {
                GoToIdle();
                
                channelingDoneAction?.Invoke();
                _isChanneling.Value = false;
            });

            void ChangeState(ChannelingState channelingState)
            {
                if (playerRefs is not PCPlayerRefs pcRefs) return;
                
                if (pcRefs.StateMachine.CanChangeStateTo<ChannelingState>())
                {
                    pcRefs.StateMachine.ChangeStateTo(channelingState);
                }
            }

            void GoToIdle()
            {
                if (playerRefs is not PCPlayerRefs pcRefs) return;
                
                if (pcRefs.StateMachine.CanChangeStateTo<IdleState>())
                {
                    pcRefs.StateMachine.ChangeStateTo<IdleState>();
                }
            }
        }

        [Server]
        public void SrvResetChanneling()
        {
            _channelingTimer.StopTimer();
            _isChanneling.Value = false;
        }
    }
}