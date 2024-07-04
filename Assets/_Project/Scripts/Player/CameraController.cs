using Cinemachine;
using Project._Project.Scripts.Player.States;
using Project._Project.Scripts.StateMachine;
using Project._Project.Scripts.UI.Settings;
using Project.Extensions;
using Sirenix.OdinInspector;
using Unity.Netcode;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Project
{
    public static class ScreenBorder
    {
        public static int left => 5;
        public static int right => Screen.width - 5;
        public static int top => Screen.height - 5;
        public static int bottom => 5;
    }
    
    public class CameraController : NetworkBehaviour
    {
        [Title("Settings")]
        [SerializeField] private float _speed = 40.0f;
        private bool _cameraLock;
        private const int _GROUND_LAYER_MASK = Constants.Layers.GroundMask;

        [Title("References")]
        [SerializeField] private Collider _border;
        private Transform _player;
        private CinemachineVirtualCamera _playerCamera;
        private StateMachineController _stateMachineController;

        private Vector3 _forward;
        private Vector3 _right;
        private Vector3 _offset;
        private float _minX;
        private float _maxX;
        private float _minZ;
        private float _maxZ;


        private void Awake()
        {
            _cameraLock = GameplaySettingsManager.cameraLock.value;
            
            _playerCamera = GameObject.FindGameObjectWithTag(Constants.Tags.Player_Camera)?.GetComponent<CinemachineVirtualCamera>();;
            _player = transform;
            
            CalculateDirectionalsVectors();
            CalculateCameraOffset();
            CalculateMinMaxAuthorizedCameraPosition();
        }

        private void OnEnable()
        {
            InputManager.instance.onCenterCamera.performed += onCenterCameraRequest_CenterCameraOnPlayer;
            InputManager.instance.onLockCamera.performed += OnCameraLockRequest_ToggleCameraLock;
        }

        private void OnDisable()
        {
            if (InputManager.IsInstanceAlive())
            {
                InputManager.instance.onCenterCamera.performed -= onCenterCameraRequest_CenterCameraOnPlayer;
                InputManager.instance.onLockCamera.performed -= OnCameraLockRequest_ToggleCameraLock;
            }
        }

        public override void OnNetworkSpawn()
        {
            base.OnNetworkSpawn();

            enabled = IsOwner;

            if (IsOwner)
            {
                _stateMachineController = GetComponentInParent<PCPlayerRefs>().StateMachine;
                
                if (IsServer)
                {
                    _stateMachineController.SrvOnStateEnter += OnEnterDeadState;
                    _stateMachineController.SrvOnStateExit += OnExitDeadState;
                }
                else if (this.IsClientOnly())
                {
                    _stateMachineController.CliOnStateEnter += OnEnterDeadState;
                    _stateMachineController.CliOnStateExit += OnExitDeadState;
                }
            }
        }

        public override void OnNetworkDespawn()
        {
            if (IsOwner)
            {
                if (IsServer)
                {
                    _stateMachineController.SrvOnStateEnter -= OnEnterDeadState;
                    _stateMachineController.SrvOnStateExit -= OnExitDeadState;
                }
                else if (this.IsClientOnly())
                {
                    _stateMachineController.CliOnStateEnter -= OnEnterDeadState;
                    _stateMachineController.CliOnStateExit -= OnExitDeadState;
                }
            }
        }

        private void Update()
        {
#if UNITY_EDITOR
            if (Application.isFocused == false) return;
#endif

            if (_cameraLock) return;
            
            ComputeMovement();
        }

        private void LateUpdate()
        {
            if (_cameraLock) CenterCameraOnPlayer();
            else ClampCameraPosition();
        }
        
        
        private void CalculateMinMaxAuthorizedCameraPosition()
        {
            if (_border == null)
            {
                _border = GameObject.FindGameObjectWithTag(Constants.Tags.Border)?.GetComponent<Collider>();
            }
            
            
            if (_border == null)
            {
                Debug.LogWarning("No border assigned, camera will have no movement limit");
                
                _minX = float.MinValue;
                _maxX = float.MaxValue;

                _minZ = _minX;
                _maxZ = _maxX;
            }
            else
            {
                Bounds borderBounds = _border.bounds;
            
                _minX = borderBounds.min.x;
                _maxX = borderBounds.max.x;
            
                _minZ = borderBounds.min.z;
                _maxZ = borderBounds.max.z;
            }
        }

        private void CalculateCameraOffset()
        {
            if (Physics.Raycast(_playerCamera.transform.position, _playerCamera.transform.forward, out RaycastHit hit,
                    Mathf.Infinity, _GROUND_LAYER_MASK))
            {
                _offset = _playerCamera.transform.position - hit.point;
                _offset.y = 0.0f;
            }
            else Debug.LogError("Unable to calculate offset");
        }

        private void CalculateDirectionalsVectors()
        {
            _forward = (_playerCamera.transform.forward + _playerCamera.transform.up).ResetAxis(EAxis.Y).normalized;
            _right = _playerCamera.transform.right;
        }

        private void ComputeMovement()
        {
            Vector2 movementInput = Vector2.zero;
            
            RetrieveMovementInputWithKeyboard(ref movementInput);
            if (GameplaySettingsManager.useMouse.value) RetrieveMovementInputWithMouse(ref movementInput);
            
            if (HasRequestedAMovement(movementInput) == false) return;
            
            Vector3 rawVelocity = _right * movementInput.x + _forward * movementInput.y;
            Vector3 velocity = rawVelocity.normalized * (_speed * Time.deltaTime);
            
            _playerCamera.transform.position += velocity;
        }

        private void RetrieveMovementInputWithKeyboard(ref Vector2 movementInput)
        {
            movementInput = InputManager.instance.move;
        }

        private void RetrieveMovementInputWithMouse(ref Vector2 movementInput)
        {
            Vector2 mousePosition = Mouse.current.position.value;

            if (mousePosition.x <= ScreenBorder.left) movementInput.x = -1.0f;
            else if (mousePosition.x >= ScreenBorder.right) movementInput.x = 1.0f;
            
            if (mousePosition.y >= ScreenBorder.top) movementInput.y = 1.0f;
            else if (mousePosition.y <= ScreenBorder.bottom) movementInput.y = -1.0f;
        }
        
        private bool HasRequestedAMovement(Vector2 movementInput)
        {
            return movementInput != Vector2.zero;
        }

        private void OnCameraLockRequest_ToggleCameraLock(InputAction.CallbackContext _)
        {
            ToggleCameraLock();
        }

        [Button]
        private void ToggleCameraLock()
        {
            _cameraLock = !_cameraLock;
            Debug.Log("Camera lock : " + _cameraLock);
        }

        private void onCenterCameraRequest_CenterCameraOnPlayer(InputAction.CallbackContext _)
        {
            CenterCameraOnPlayer();
        }

        [Button]
        public void CenterCameraOnPlayer()
        {
            _playerCamera.transform.position = new Vector3(_player.transform.position.x, _playerCamera.transform.position.y, _player.transform.position.z) + _offset;
        }
        
        private void ClampCameraPosition()
        {
            Vector3 cameraPosition = _playerCamera.transform.position;
            
            Vector3 clampedPosition = new Vector3(
                Mathf.Clamp(cameraPosition.x, _minX, _maxX),
                cameraPosition.y,
                Mathf.Clamp(cameraPosition.z, _minZ, _maxZ)
                );
            
            _playerCamera.transform.position = clampedPosition;
        }
        
        private void OnEnterDeadState(BaseStateMachineBehaviour state)
        {
            if (state is DeadState) enabled = false;
        }
        
        private void OnExitDeadState(BaseStateMachineBehaviour state)
        {
            if (state is DeadState)
            {
                CenterCameraOnPlayer();
                enabled = true;
            }
        }
    }
}
