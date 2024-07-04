using System;
using UnityEngine;

namespace Project.Spells.Casters
{
    public class SkillShotCaster : SpellCaster
    {
        public override Type CastResultType => typeof(SingleVectorResults);
        public override Type SpellDataType => typeof(SkillshotSpellData);
        
        [SerializeField] private Transform visual, visualParent;
        
        [SerializeField] private LayerMask groundLayerMask;
        private Camera _camera;
        
        private SkillshotSpellData _skillshotSpellData;
        private SingleVectorResults _currentResults = new();

        private void Start()
        {
            _camera = Camera.main;
            visualParent.gameObject.SetActive(false);
        }
        
        public override void Init(PlayerRefs caster, SpellData spell)
        {
            base.Init(caster, spell);
            
            var skillshotSpell = spell as SkillshotSpellData;

            if (skillshotSpell == null)
            {
                Debug.LogError("SkillshotCaster can only be used with SkillshotSpellData. You gave spell: " +
                               spell.name + " of Type: " + spell.GetType());
                return;
            }

            _skillshotSpellData = skillshotSpell;
            
            var scale = visual.localScale;
            scale.y = _skillshotSpellData.length;

            var pos = visual.localPosition;
            pos.z += (scale.y - 1) * 0.5f;
            
            visual.localScale = scale;
            visual.localPosition = pos;
        }
        
        public override void StartCasting()
        {
            if (IsCasting) return;
            
            base.StartCasting();
            visualParent.gameObject.SetActive(true);
        }
        
        public override void StopCasting()
        {
            if (!IsCasting) return;
            
            base.StopCasting();
            visualParent.gameObject.SetActive(false);
        }
        
        protected override void UpdateCasting()
        {
            var dir = _currentResults.VectorProp - CasterTransform.position;
            dir.y = 0;
            dir.Normalize();
            
            visualParent.rotation = Quaternion.LookRotation(dir);
        }

        public override void EvaluateResults()
        {
            Utilities.GetMouseWorldPosition(_camera, groundLayerMask, out Vector3 position);
            position.y = 0;
            
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

            visualParent.gameObject.SetActive(true);
            return true;
        }

        public override bool StopPreview()
        {
            if (base.StopPreview()) return true;
            
            visualParent.gameObject.SetActive(false);
            return true;
        }
    }
}