using Sirenix.OdinInspector;
using UnityEngine;

namespace Project.Spells
{
    [CreateAssetMenu(fileName = "New TargetData", menuName = "Spells/Data/Target", order = 1)]
    public class SkillshotSpellData : SpellData
    {
        [BoxGroup("Skillshot Data")] public float length;
    }
}