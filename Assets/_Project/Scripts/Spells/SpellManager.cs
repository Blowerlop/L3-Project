using System;
using System.Collections.Generic;
using Project._Project.Scripts.Player.States;
using Unity.Netcode;
using UnityEngine;

namespace Project.Spells
{
    public class SpellManager : NetworkSingleton<SpellManager>
    {
        public static event Action<PlayerRefs, Vector3> OnChannelingStarted;
        
        private List<Timer> _runningCasts = new();
        
        [Server]
        public void TryCastSpell(int clientId, int spellIndex, ICastResult results)
        {
            UserInstance user = UserInstanceManager.instance.GetUserInstance(clientId);
            if (user == null)
            {
                Debug.LogError("Trying to cast a spell for an invalid user.");
                return;
            }

            PlayerRefs playerRefs = user.LinkedPlayer;
            if (playerRefs == null)
            {
                Debug.LogError("Trying to cast a spell for a user that have no player assigned.");
                return;
            }

            if (!TryGetSpellData(user, spellIndex, out var spell)) return;

            Debug.Log($"Trying to cast spell {spell.spellId} for {user.ClientId} at index {spellIndex}.");
            
            CooldownController cooldownController = playerRefs.Cooldowns;
            if (cooldownController.IsInCooldown(spellIndex)) return;
            
            if (!CanCastSpell(playerRefs))
            {
                UnableToCastCallback();
                return;
            }
            
            ChannelingController channelingController = playerRefs.Channeling;
            if (channelingController.IsChanneling)
            {
                UnableToCastCallback();
                return;
            }
            
            cooldownController.StartServerCooldown(spellIndex, spell.cooldown);

            channelingController.StartServerChanneling(spell.channelingTime, (byte)spellIndex,
                () => OnChannelingEnded(spell, spellIndex, results, playerRefs));

            var dir = spell.instantiationType == SpellInstantiationType.None ? 
                playerRefs.PlayerTransform.forward 
                : spell.spellPrefab.GetDirection(results, playerRefs);
            
            OnChannelingStarted?.Invoke(playerRefs, dir);

            return;

            void UnableToCastCallback()
            {
                cooldownController.ChangeNegativeValue(spellIndex);
            }
        }

        [Server]
        private void OnChannelingEnded(SpellData spell, int spellIndex, ICastResult results, PlayerRefs playerRefs)
        {
            switch (spell.instantiationType)
            {
                case SpellInstantiationType.NetworkObject:
                    HandleNetworkObjectSpawn();
                    break;
                case SpellInstantiationType.ServerOnly:
                    HandleServerOnlySpawn();
                    break;
                case SpellInstantiationType.None:
                    HandleNoPrefabSpell();
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            HandleCastAnimation(spell, spellIndex, playerRefs);

            return;
            
            void HandleNetworkObjectSpawn()
            {
                Spell spellInstance = SpawnSpell(spell, results, playerRefs);
                spellInstance.Init(results, playerRefs, serverOnly: false);
            }
            
            void HandleServerOnlySpawn()
            {
                Spell spellInstance = SpawnSpell(spell, results, playerRefs, onNetwork: false);
                spellInstance.Init(results, playerRefs, serverOnly: true);
            }
            
            void HandleNoPrefabSpell()
            {
                var playerEntity = playerRefs.GetPC().Entity;
            
                foreach (var effect in spell.effects)
                    effect.GetInstance().TryApply(playerEntity, applier: playerRefs, applyPosition: default);
            }
        }
        
        [Server]
        private void HandleCastAnimation(SpellData spell, int spellIndex, PlayerRefs playerRefs)
        {
            if (spell.castAnimationDuration <= 0) return;
            
            var boolName = "Cast " + (spellIndex + 1);
            
            playerRefs.NetworkAnimator.Animator.SetBool(boolName, true);
            
            var pcPlayer = playerRefs as PCPlayerRefs;
            if (pcPlayer)
            {
                pcPlayer.InCastController.SrvSetInCast(spellIndex);
                
                // Stop player if cast doesn't allow movements
                // or if player can move during cast but is not actually moving
                if (!spell.castingFlags.HasFlag(CastingFlags.EnableMovements) 
                    || pcPlayer.StateMachine.currentState is not MoveState)
                {
                    if (pcPlayer.StateMachine.CanChangeStateTo<IdleState>())
                    {
                        pcPlayer.StateMachine.ChangeStateTo<IdleState>();
                    }
                }
            }

            var timer = GetCastTimer();
            
            timer.StartTimerWithCallback(this, spell.castAnimationDuration, OnCastEnd);
            
            return;

            void OnCastEnd()
            {
                playerRefs.NetworkAnimator.Animator.SetBool(boolName, false);
                
                if (pcPlayer) pcPlayer.InCastController.SrvResetInCast();
            }
        }

        private Timer GetCastTimer()
        {
            foreach (var timer in _runningCasts)
                if (!timer.isTimerRunning) return timer;
            
            Timer newTimer = new Timer();
            _runningCasts.Add(newTimer);
            
            return newTimer;
        }

        [Server]
        private bool TryGetSpellData(UserInstance user, int spellIndex, out SpellData spell)
        {
            spell = null;
            
            if (!SOCharacter.TryGetCharacter(user.CharacterId, out var characterData))
            {
                Debug.LogError($"Trying to cast a spell for an invalid character {user.CharacterId}.");
                return false;
            }
            if (!characterData.TryGetSpell(spellIndex, out spell))
            {
                Debug.LogError($"Trying to cast an invalid spell : {spellIndex} of character " +
                               $"{characterData.characterName}.");
                return false;
            }

            return true;
        }
        
        [Server]
        private Spell SpawnSpell(SpellData spell, ICastResult results, PlayerRefs playerRefs, bool onNetwork = true)
        {
            Spell spellPrefab = spell.spellPrefab;
            
            (Vector3 pos, Quaternion rot) trans = spellPrefab.GetDefaultTransform(results, playerRefs);
            
            Spell spellInstance = Instantiate(spellPrefab, trans.pos, trans.rot);
            if (onNetwork) spellInstance.NetworkObject.Spawn();
            
            return spellInstance;
        }

        #region Utils

        public static bool CanCastSpell(PlayerRefs refs)
        {
            return Gameloop.IsGameRunning && (refs is not PCPlayerRefs pcPlayer
                   || (pcPlayer.StateMachine.CanChangeStateTo<ChannelingState>() 
                       && !pcPlayer.Entity.IsSilenced 
                       && !pcPlayer.InCastController.IsCasting));
        }

        [Server]
        public void SrvResetSpells()
        {
            var spells = FindObjectsByType<Spell>(FindObjectsInactive.Exclude, FindObjectsSortMode.None);

            foreach (var spell in spells)
            {
                if(spell.NetworkObject.IsSpawned)
                    spell.NetworkObject.Despawn();
                else
                    Destroy(spell.gameObject);
            }
        }

        [Server]
        public void SrvResetCasts()
        {
            foreach (var castTimer in _runningCasts)
            {
                castTimer.StopTimer();
            }
        }
        
        #endregion
        
        #region TryCast Interface

        //Since Netcode doesn't really handle polymorphism, we need to create a TryCast method for each channeling results
        
        [ServerRpc(RequireOwnership = false)]
        public void TryCastSpellServerRpc(int spellIndex, SingleVectorResults results, ServerRpcParams serverRpcParams = default)
        {
            TryCastSpell((int)serverRpcParams.Receive.SenderClientId, spellIndex, results);
        }
        
        [ServerRpc(RequireOwnership = false)]
        public void TryCastSpellServerRpc(int spellIndex, EmptyResults results, ServerRpcParams serverRpcParams = default)
        {
            TryCastSpell((int)serverRpcParams.Receive.SenderClientId, spellIndex, results);
        }
        
        [ServerRpc(RequireOwnership = false)]
        public void TryCastSpellServerRpc(int spellIndex, IntResults results, ServerRpcParams serverRpcParams = default)
        {
            TryCastSpell((int)serverRpcParams.Receive.SenderClientId, spellIndex, results);
        }

        #endregion
    }
}