using _Project._200_Dev.Entities.Player;
using _Project._200_Dev.Entities.Player.Interfaces;
using _Project._200_Dev.Networking.IL_Weaving.Server;
using Unity.Netcode;
using UnityEngine;

namespace _Project._200_Dev.Entities.Projectile
{
    public class Projectile : NetworkBehaviour
    {
        private AttackController _attackController;
        private Transform _target;
        private float _speed;

        private static Vector3 offset = new Vector3(0, 3f, 0); //Hit the torso not the feet

        public override void OnNetworkSpawn()
        {
            enabled = IsServer;
        }

        private void FixedUpdate()
        {
            if (IsSpawned == false) return;
            Vector3 adjustedTargetPos = _target.position + offset;
            transform.LookAt(adjustedTargetPos);
            Vector3 direction = adjustedTargetPos - transform.position;
            Vector3 directionNormalized = direction.normalized;
            
            transform.position += directionNormalized * (Time.fixedDeltaTime * _speed); 
            if (direction.sqrMagnitude < SOProjectile.HIT_RANGE * SOProjectile.HIT_RANGE || (directionNormalized * (Time.fixedDeltaTime * _speed)).magnitude > direction.magnitude)
            {
                // Projectile follow an IEffectable target. Don't need to check if it's null.
                _attackController.Hit(_target.GetComponent<IEffectable>());
                GetComponent<NetworkObject>().Despawn();
            }
        }
        
        
        public void Init(AttackController attackController, SOProjectile projectileData)
        {
            _attackController = attackController;
            _target = attackController.targetNetworkObject.transform;
            _speed = projectileData._speed;
        }

        [Server]
        public static void SrvResetProjectiles()
        {
            var projectiles = FindObjectsByType<Projectile>(FindObjectsInactive.Include, FindObjectsSortMode.None);
            
            foreach(var projectile in projectiles)
            {
                projectile.NetworkObject.Despawn();
            }
        }
    }
}