using System;
using Project._Project.Scripts;
using UnityEngine;

namespace Project.Spells.Casters
{
    public class TargetCaster : SpellCaster
    {
        public override Type CastResultType => typeof(IntResults);
        public override Type SpellDataType => typeof(TargetSpellData);
        
        [SerializeField] private Transform zoneVisual;
        
        private IntResults _currentResults = new() { IntProp = -1 };

        private TargetSpellData _targetSpellData;
        private Camera _camera;
        
        private void Start()
        {
            _camera = Camera.main;
            zoneVisual.gameObject.SetActive(false);
        }

        public override void Init(PlayerRefs caster, SpellData spell)
        {
            base.Init(caster, spell);
            
            var targetSpell = spell as TargetSpellData;

            if (targetSpell == null)
            {
                Debug.LogError("TargetCaster can only be used with TargetSpellData. You gave spell: " +
                               spell.name + " of Type: " + spell.GetType());
                return;
            }

            _targetSpellData = targetSpell;
            
            zoneVisual.localScale = Vector3.one * _targetSpellData.limitRadius * 2;
        }
        
        public override void StartCasting()
        {
            if (IsCasting) return;
            
            base.StartCasting();
            zoneVisual.gameObject.SetActive(true);
            
            _currentResults.IntProp = -1;
        }
        
        public override void StopCasting()
        {
            if (!IsCasting) return;
            
            base.StopCasting();
            zoneVisual.gameObject.SetActive(false);
        }
        
        protected override void UpdateCasting() { }

        public override void EvaluateResults()
        {
            if (!Utilities.GetMouseWorldHit(_camera, Constants.Layers.EntityMask, out RaycastHit hitInfo))
            {
                InvalidTarget();
                return;
            }

            if (!hitInfo.transform.TryGetComponent<Entity>(out var entity))
            {
                InvalidTarget();
                return;
            }

            switch (_targetSpellData.targetType)
            {
                case SpellTargetType.Enemy:
                    if (entity.TeamIndex == Caster.TeamIndex)
                    {
                        InvalidTarget();
                        return;
                    }
                    break;
                case SpellTargetType.Ally:
                    if (entity.TeamIndex != Caster.TeamIndex)
                    {
                        InvalidTarget();
                        return;
                    }
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            if (Vector3.Distance(entity.transform.position, CasterTransform.position) > _targetSpellData.limitRadius)
            {
                InvalidTarget();
                return;
            }
            
            _currentResults.IntProp = (int)entity.NetworkObjectId;

            return;

            void InvalidTarget()
            {
                _currentResults.IntProp = -1;
            }
        }

        public override bool TryCast(int casterIndex)
        {
            if (_currentResults.IntProp == -1)
                return false;
            
            SpellManager.instance.TryCastSpellServerRpc(casterIndex, _currentResults);
            return true;
        }

        public override bool Preview()
        {
            if (base.Preview()) return true;
            
            zoneVisual.gameObject.SetActive(true);
            return true;
        }

        public override bool StopPreview()
        {
            if (base.StopPreview()) return true;
            
            zoneVisual.gameObject.SetActive(false);
            return true;
        }
    }
}