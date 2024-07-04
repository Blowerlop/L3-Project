using Unity.Netcode;
using UnityEngine;

namespace Project.Spells
{
    public class InCastController : NetworkBehaviour
    {
        [SerializeField] private PCPlayerRefs playerRefs;
        
        public bool IsCasting => _castedSpell.Value >= 0;
        private NetworkVariable<int> _castedSpell = new(-1);
        
        public CastingFlags CastingFlags { get; private set; }
        
        public override void OnNetworkSpawn()
        {
            _castedSpell.OnValueChanged += OnCastedSpellChanged;
        }
        
        private void OnCastedSpellChanged(int _, int newIndex)
        {
            if (newIndex < 0) return;
            // Maybe this should not be here? Or every entity should have spells?
            if (playerRefs.Entity.data is not SOCharacter characterData) return;

            if (!characterData.TryGetSpell(newIndex, out var spellData)) return;
            
            CastingFlags = spellData.castingFlags;
        }
        
        [Server]
        public void SrvSetInCast(int spellIndex)
        {
            _castedSpell.Value = spellIndex;
        }
        
        [Server]
        public void SrvResetInCast()
        {
            _castedSpell.Value = -1;
        }
    }
}