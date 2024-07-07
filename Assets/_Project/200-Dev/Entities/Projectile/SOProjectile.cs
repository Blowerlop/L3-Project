using Sirenix.OdinInspector;
using UnityEngine;

namespace _Project._200_Dev.Entities.Projectile
{
    [CreateAssetMenu(menuName = "Scriptable Objects/Projectile")]
    public class SOProjectile : ScriptableObject
    {
        [field: SerializeField, AssetsOnly, PreviewField] public Projectile prefab { get; private set; }
        [SerializeField] public float _speed = 1.0f;
        public const float HIT_RANGE = 0.1f;
    }
}