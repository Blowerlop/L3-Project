using System;
using UnityEngine;

namespace Project.Spells.Casters
{
    public class ZoneCaster : SpellCaster
    {
        public override Type CastResultType => typeof(SingleVectorResults);
        public override Type SpellDataType => typeof(ZoneSpellData);

        [SerializeField] private Transform aimVisual;
        [SerializeField] private Transform zoneVisual;
        
        private Camera _camera;
        private ZoneSpellData _zoneSpellData;
        
        private SingleVectorResults _currentResults = new();
        
        private void Start()
        {
            _camera = Camera.main;
            zoneVisual.gameObject.SetActive(false);
            aimVisual.gameObject.SetActive(false);
        }

        public override void Init(PlayerRefs caster, SpellData spell)
        {
            base.Init(caster, spell);
            
            var zoneSpell = spell as ZoneSpellData;

            if (zoneSpell == null)
            {
                Debug.LogError("DefaultZoneCaster can only be used with DefaultZoneData. You gave spell: " +
                               spell.name + " of Type: " + spell.GetType());
                return;
            }

            _zoneSpellData = zoneSpell;
            
            zoneVisual.localScale = Vector3.one * zoneSpell.limitRadius * 2;
            aimVisual.localScale = Vector3.one * zoneSpell.zoneRadius * 2;
        }
        
        public override void StartCasting()
        {
            if (IsCasting) return;
            
            base.StartCasting();
            zoneVisual.gameObject.SetActive(true);
            aimVisual.gameObject.SetActive(true);
        }
        
        public override void StopCasting()
        {
            if (!IsCasting) return;
            
            base.StopCasting();
            zoneVisual.gameObject.SetActive(false);
            aimVisual.gameObject.SetActive(false);
        }
        
        protected override void UpdateCasting()
        {
            var pos = _currentResults.VectorProp;
            pos.y = aimVisual.position.y;
            
            aimVisual.position = pos;
            
            if (_zoneSpellData.lookAtCenter)
            {
                var dir = pos - zoneVisual.position;
                dir.y = 0;
                dir.Normalize();
                
                aimVisual.rotation = Quaternion.LookRotation(dir);
            }
        }

        public override void EvaluateResults()
        {
            Utilities.GetMouseWorldPosition(_camera, Constants.Layers.GroundMask, out Vector3 position);
            
            var zoneCenter = zoneVisual.position;
            position = zoneCenter + Vector3.ClampMagnitude(position - zoneCenter, _zoneSpellData.limitRadius);
            
            _currentResults.VectorProp = position;
        }

        public override bool TryCast(int casterIndex)
        {
            SpellManager.instance.TryCastSpellServerRpc(casterIndex, _currentResults);
            return true;
        }

        public override bool Preview()
        {
            if (base.Preview()) return true;
            
            zoneVisual.gameObject.SetActive(true);
            aimVisual.gameObject.SetActive(true);
            
            var pos = zoneVisual.position;
            pos.y = aimVisual.position.y;
            
            aimVisual.position = pos;
            return true;
        }

        public override bool StopPreview()
        {
            if (base.StopPreview()) return true;
            
            zoneVisual.gameObject.SetActive(false);
            aimVisual.gameObject.SetActive(false);
            return true;
        }
    }
}