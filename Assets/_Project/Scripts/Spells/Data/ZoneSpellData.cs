using Sirenix.OdinInspector;
using UnityEngine;

namespace Project.Spells
{
    [CreateAssetMenu(fileName = "New ZoneData", menuName = "Spells/Data/Zone", order = 1)]
    public class ZoneSpellData : SpellData
    {
        [BoxGroup("Zone Data")] public float limitRadius;
        [BoxGroup("Zone Data")] public float zoneRadius;
        
        [BoxGroup("Zone Data")] public bool lookAtCenter;
    }
}