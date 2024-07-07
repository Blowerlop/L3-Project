using System;
using Sirenix.OdinInspector;
using UnityEngine;

namespace Project.Spells.Casters
{
    public abstract class SpellCaster : MonoBehaviour
    {
        public abstract Type CastResultType { get; }
        public abstract Type SpellDataType { get; }
        
        [ShowInInspector, ReadOnly, PropertyOrder(-1), LabelText("Cast Result Type")] 
        private string CastResultTypeAsString => CastResultType.Name;
        [ShowInInspector, ReadOnly, PropertyOrder(-1), LabelText("Spell Data Type")] 
        [PropertySpace(SpaceBefore = 0, SpaceAfter = 15)]
        private string SpellDataTypeAsString => SpellDataType.Name;
        
        public bool IsCasting { get; private set; }

        protected PlayerRefs Caster { get; private set; }
        protected Transform CasterTransform { get; private set; }

        private SpellData _data;
        private GameObject _previewOverride;
        
        public virtual void Init(PlayerRefs caster, SpellData spell)
        {
            Caster = caster;
            CasterTransform = caster.PlayerTransform;
            
            _data = spell;
        }
        
        public virtual void StartCasting()
        {
            if (IsCasting) return;
            
            IsCasting = true;
            
            StopPreview();
        }
        
        public virtual void StopCasting()
        {
            if (!IsCasting) return;
            
            IsCasting = false;
        }
        
        protected virtual void Update()
        {
            if (!IsCasting) return;
            
            EvaluateResults();
            UpdateCasting();
        }

        /// <summary>
        /// Called every frame if this caster is channeling.
        /// Called after EvaluateResults().
        /// </summary>
        protected abstract void UpdateCasting();

        /// <summary>
        /// Should evaluate current results based on user inputs and other factors. Then update _currentResults.
        /// Called before UpdateChanneling().
        /// </summary>
        public abstract void EvaluateResults();

        /// <summary>
        /// Ask SpellManager to spawn the desired spell with caster current results.
        /// </summary>
        /// <param name="casterIndex"></param>
        public abstract bool TryCast(int casterIndex);

        public virtual bool Preview()
        {
            if (!_data.overrideCasterPreview) return false;
            
            _previewOverride ??= Instantiate(_data.overridePreviewPrefab, transform);
            _previewOverride.SetActive(true);

            return true;
        }

        public virtual bool StopPreview()
        {                        
            if (!_data.overrideCasterPreview) return false;
            if (!_previewOverride) return false;
            
            _previewOverride.SetActive(false);

            return true;
        }
    }
}