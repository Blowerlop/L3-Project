using Sirenix.OdinInspector;
using UnityEngine;

namespace _Project._200_Dev.Spells.Data
{
    [CreateAssetMenu(fileName = "New TargetData", menuName = "Spells/Data/Target", order = 1)]
    public class TargetSpellData : SpellData
    {
        [BoxGroup("Target Data")] public SpellTargetType targetType;
        [BoxGroup("Target Data")] public float limitRadius;
    }

    public enum SpellTargetType
    {
        Enemy,
        Ally
    }
}