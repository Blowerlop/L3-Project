using Project._Project.Scripts;
using Project._Project.Scripts.Player.States;
using Project._Project.Scripts.StateMachine;
using Project.Extensions;
using Project.Spells;
using Project.Spells.Casters;
using UnityEngine;
using UnityEngine.AI;

namespace Project
{
    public class PCPlayerRefs : PlayerRefs
    {
        [SerializeField] private Entity _entity;
        
        [SerializeField] private StateMachineController _stateMachine;
        [SerializeField] private NavMeshAgent _navMeshAgent;
        [SerializeField] private PlayerMouse _playerMouse;
        [SerializeField] private MovementController _movementController;
        [SerializeField] private SpellCastController spellCastController;
        
        [SerializeField] private InCastController inCastController;
        
        [SerializeField] private EmoteController emoteController;
        
        [SerializeField] private AttackController attackController;
        
        public Entity Entity => _entity;
        public StateMachineController StateMachine => _stateMachine;
        public NavMeshAgent NavMeshAgent => _navMeshAgent;
        public PlayerMouse PlayerMouse => _playerMouse;
        public MovementController MovementCOntroller => _movementController;

        public SpellCastController SpellCastController => spellCastController; 
        public InCastController InCastController => inCastController;
        
        public EmoteController EmoteController => emoteController;
        
        public AttackController AttackController => attackController;
        
        public override PCPlayerRefs GetPC() => this;

        [Server]
        public override void ServerInit(int team, int ownerId, SOEntity entity)
        {
            base.ServerInit(team, ownerId, entity);
            
            _entity.ServerInit(entity);
        }
        
        protected override void OnOwnerChanged(int oldId, int newId)
        {
            base.OnOwnerChanged(oldId, newId);
            
            if (!IsClient) return;
            
            spellCastController.Init(this);
            
            if(UserInstance.Me.ClientId == newId)
                GetComponentInChildren<CameraController>().CenterCameraOnPlayer();
        }

        [Server]
        public override void SrvResetPlayer()
        {
            base.SrvResetPlayer();
            
            _entity.SrvResetEntity();
            _stateMachine.ChangeStateTo<IdleState>();
            attackController.SrvForceReset();
            inCastController.SrvResetInCast();
        }

        protected override void OnTeamChanged(int oldValue, int newValue)
        {
            base.OnTeamChanged(oldValue, newValue);

            if (this.IsClientOnly())
            {
                bool isAlly = UserInstance.Me.Team == newValue;
                Entity.SetOutlineColor(isAlly);
            }
        }
    }
}