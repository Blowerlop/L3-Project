using System;
using System.Collections.Generic;
using System.Linq;
using _Project._200_Dev.Entities.Player.Interfaces;
using _Project._200_Dev.Entities.Player.Stats;
using _Project._200_Dev.Networking.IL_Weaving.Server;
using _Project._200_Dev.UI;
using Sirenix.OdinInspector;
using Unity.Netcode;
using UnityEngine;

namespace _Project._200_Dev.Entities
{
    public abstract class Entity : NetworkBehaviour, IHealable, IDamageable, IShieldable, IEffectable, ISilenceable/*, ITargetable*/
    {
        [field: ShowInInspector, ReadOnly] public SOEntity data { get; private set; }
        [SerializeField] protected PlayerStats _stats;
        
        [Header("Outline")]
        
        [SerializeField] private MeshOutline outline;
        
        [SerializeField] private float outlineShowWidth;
    
        [SerializeField] private float outlineShowDuration;
        [SerializeField] private float outlineHideDuration;
    
        [SerializeField] private Color allyOutlineColor = Color.green;
        [SerializeField] private Color enemyOutlineColor = Color.red;
        
        public PlayerStats Stats => _stats;

        public virtual int TeamIndex => -1;

        public Entity AffectedEntity => this;
        [ShowInInspector, ReadOnly] public IList<Effect> AppliedEffects { get; } = new List<Effect>();
        
        public bool IsHovered { get; private set; }
        
        public bool IsSilenced => _isSilenced.Value;
        private NetworkVariable<bool> _isSilenced = new();
        private int _silenceAmount;
        public event Action<bool> OnSilenceChanged;
        
        private int _stunAmount;
        
        private bool _isInit;
        private event Action _onEntityInit;
        public event Action onEntityInit
        {
            add
            {
                if (_isInit)
                {
                    value.Invoke();
                }
                else _onEntityInit += value;
            }
            remove => _onEntityInit -= value;
        }

        public override void OnNetworkSpawn()
        {
            base.OnNetworkSpawn();
            
            SilenceChanged(false, _isSilenced.Value);
            _isSilenced.OnValueChanged += SilenceChanged;
            
            if (!_stats.isInitialized) _stats.OnStatsInitialized += OnStatsInitialized;
            else OnStatsInitialized();
        }

        public override void OnNetworkDespawn()
        {
            base.OnNetworkDespawn();
            
            _isSilenced.OnValueChanged -= SilenceChanged;
            
            if(_stats is { nHealthStat: not null })
                _stats.nHealthStat._nValue.OnValueChanged -= OnHealthChanged;
        }

        [Server]
        public void ServerInit(SOEntity entityData)
        {
            data = entityData;
            _stats.ServerInit(entityData);
            
            _onEntityInit?.Invoke();
            _onEntityInit = null;
            _isInit = true;
        }
        
        private void OnStatsInitialized()
        {
            _stats.OnStatsInitialized -= OnStatsInitialized;
            
            _stats.nHealthStat._nValue.OnValueChanged += OnHealthChanged;
        }
        
        public void Heal(int modifier)
        {
            _stats.nHealthStat.Value += modifier;
        }

        public void MaxHeal()
        {
            _stats.nHealthStat.SetToMaxValue();
        }
        
        // Can do this with health because we don't need attacker info for the animation
        public void OnHealthChanged(int oldValue, int newValue)
        {
            if (!HealthTextPool.instance) return;
            
            var diff = newValue - oldValue;
            
            if(diff >= 0)
                HealthTextPool.instance.RequestText(diff, transform, default);
        }

        public void Damage(int modifier)
        {
            CheckForShieldDamage(ref modifier);
            
            _stats.nHealthStat.Value -= modifier;
        }

        [ClientRpc]
        public void OnDamagedByClientRpc(ushort attackerId, int amount)
        {
            if (!HealthTextPool.instance) return;

            if (!NetworkManager.Singleton.SpawnManager.SpawnedObjects.TryGetValue(attackerId, out var attacker))
                return;
            
            var dir = transform.position - attacker.transform.position;
            dir.y = 0;
            dir.Normalize();
            
            HealthTextPool.instance.RequestText(-amount, transform, dir);
        }

        private void CheckForShieldDamage(ref int modifier)
        {
            if (_stats.nShieldStat == null || !_stats.nShieldStat.HasShield) return;

            var lastValue = _stats.nShieldStat.Value;
            _stats.nShieldStat.Value -= modifier;
            
            modifier -= lastValue;
            if(modifier < 0) modifier = 0;
        }
        
        public bool CanDamage(int teamIndex)
        {
            return TeamIndex != teamIndex;
        }
        
        public int Shield(int modifier)
        {
            var shield = new ShieldData(modifier);
            
            _stats.nShieldStat.AddShield(shield);
            return shield.ID;
        }

        public void UnShield(int shieldId)
        {
            _stats.nShieldStat.RemoveShield(shieldId);
        }
        
        #region Effects
        
        [Server]
        public void SrvAddEffect(Effect effect)
        {
            AppliedEffects.Add(effect);
        }
        
        [Server]
        public void SrvRemoveEffect(Effect effect)
        {
            AppliedEffects.Remove(effect);
        }
        
        [Server]
        public void SrvCleanse()
        {
            KillEffectsOfType(EffectType.Bad);
        }

        [Server]
        public void SrvDebuff()
        {
            KillEffectsOfType(EffectType.Good);
        }

        private void KillEffectsOfType(EffectType type)
        {
            var copy = new List<Effect>(AppliedEffects);
            foreach (var appliedEffect in copy.Where(effect => effect.Type == type))
            {
                appliedEffect.KillEffect();
            }
        }
        
        #endregion

        #region Silence
        
        public void Silence()
        {
            _silenceAmount++;
            _isSilenced.Value = true;
        }

        public void Unsilence()
        {
            _silenceAmount--;

            if (_silenceAmount <= 0)
            {
                _silenceAmount = 0;
                _isSilenced.Value = false;
            }
        }

        private void SilenceChanged(bool oldValue, bool newValue)
        {
            OnSilenceChanged?.Invoke(newValue);
        }
        
        #endregion
        
        #region Stun
        
        public void Stun()
        {
            _stunAmount++;
            
            // If it's the first stun
            if (_stunAmount == 1)
                OnStunned();
        }

        protected virtual void OnStunned()
        {
            // Override
        }
        
        public void UnStun()
        {
            _stunAmount--;
            
            if (_stunAmount <= 0)
            {
                _stunAmount = 0;
                OnUnStunned();
            }
        }
        
        protected virtual void OnUnStunned()
        {
            // Override
        }
        
        #endregion

        public void SrvResetEntity()
        {
            _isSilenced.Value = false;

            foreach (var effect in AppliedEffects.ToList())
            {
                effect.KillEffect();
            }
            
            _stats.SrvResetStats();
        }
        
        #region Hover
        
        public void OnHover()
        {
            outline.Show(outlineShowWidth, outlineShowDuration);
            IsHovered = true;
        }

        public void OnUnhover()
        {
            outline.Hide(outlineHideDuration);
            IsHovered = false;
        }
        
        public void SetOutlineColor(bool isAlly)
        {
            outline.SetColor(isAlly ? allyOutlineColor : enemyOutlineColor);
        }
        
        #endregion

    }
}