using System;
using _Project._200_Dev.Networking.IL_Weaving.Server;
using _Project._200_Dev.Spells;
using _Project._200_Dev.Spells.Cooldowns;
using _Project._200_Dev.User;
using Unity.Netcode;
using Unity.Netcode.Components;
using UnityEngine;

namespace _Project._200_Dev.Entities.Player
{
    [GenerateSerializationForType(typeof(byte))]
    public abstract class PlayerRefs : NetworkBehaviour
    {
        private NetworkVariable<int> _assignedTeam = new(-1);
        private NetworkVariable<int> _ownerId = new(int.MaxValue);

        [SerializeField] protected Transform playerTransform;
        [SerializeField] protected Transform shootTransform;
        
        [SerializeField] private CooldownController cooldowns;
        [SerializeField] private ChannelingController channeling;
        
        [SerializeField] private NetworkAnimator _networkAnimator;
        
        [SerializeField] private PlayerSpellsAnimFXHandler playerSpellsAnimFxHandler;
        
        public int TeamIndex => _assignedTeam.Value;
        public int OwnerId => _ownerId.Value;
        
        public Transform PlayerTransform => playerTransform;
        public Transform ShootTransform => shootTransform;
        
        public CooldownController Cooldowns => cooldowns;
        public ChannelingController Channeling => channeling;
        
        public NetworkAnimator NetworkAnimator => _networkAnimator;

        public event Action<int> OnTeamChangedCallback;
        
        /// <returns>
        /// PCPlayerRefs linked to this PlayerRefs.
        /// If this is a mobile, this will return linked PC.
        /// If this is a PC, this will return itself.
        /// </returns>
        public abstract PCPlayerRefs GetPC();

        #region Team Linking
        
        // Is done before ServerInit on server
        public override void OnNetworkSpawn()
        {
            base.OnNetworkSpawn();
            
            
            // OnValueChanged is not called for network object that were already spawned before joining
            // We need to call OnTeamChanged manually, if the value is not the default one
            if(_assignedTeam.Value != -1) OnTeamChanged(-1, _assignedTeam.Value);
            _assignedTeam.OnValueChanged += OnTeamChanged;
            
            if(_ownerId.Value != int.MaxValue) OnOwnerChanged(int.MaxValue, _ownerId.Value);
            _ownerId.OnValueChanged += OnOwnerChanged;
        }

        public override void OnNetworkDespawn()
        {
            base.OnNetworkDespawn();
            
            _assignedTeam.OnValueChanged -= OnTeamChanged;
            _ownerId.OnValueChanged -= OnOwnerChanged;
        }

        [Server]
        public virtual void ServerInit(int team, int ownerId, SOEntity entity)
        {
            _ownerId.Value = ownerId;
            _assignedTeam.Value = team;
        }

        protected virtual void OnTeamChanged(int oldValue, int newValue) { OnTeamChangedCallback?.Invoke(newValue); }
        
        protected virtual void OnOwnerChanged(int oldId, int newId)
        {
            if(UserInstanceManager.instance.TryGetUserInstance(oldId, out var oldUser))
            {
                oldUser.UnlinkPlayer();
            }
            
            if(UserInstanceManager.instance.TryGetUserInstance(newId, out var newUser))
            {
                newUser.LinkPlayer(this);
            }
            
            PlayerManager.instance.SetPlayerReady();
        }
        
        #endregion

        [Server]
        public virtual void SrvResetPlayer()
        {
            // Force reset all animator
            NetworkAnimator.Animator.Rebind();

            if (playerSpellsAnimFxHandler)
            {
                playerSpellsAnimFxHandler.ResetFX();
                ResetFXClientRpc();
            }
            
            channeling.SrvResetChanneling();
            cooldowns.SrvResetCooldowns();
        }

        [ClientRpc]
        private void ResetFXClientRpc()
        {
            if (playerSpellsAnimFxHandler)
                playerSpellsAnimFxHandler.ResetFX();
        }
    }
}