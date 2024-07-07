using System;
using System.Collections.Generic;
using Project._Project.Scripts;
using Unity.Netcode;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Project
{
    public class PlayerMouse : NetworkBehaviour
    {
        private enum CursorType
        {
            Default,
            Attack,
            Spell,
            Forbidden
        }

        [Serializable]
        private struct CursorWrapper
        {
            public CursorType type;
            public CursorData data;
        }
        
        [Serializable]
        private struct CursorData
        {
            public Texture2D texture;
            public Vector2 hotspot;
        }
        
        /// <summary>
        /// Owner authoritative event for when the player clicks the mouse and hits something.
        /// 2nd argument is the layer index of the object hit.
        /// </summary>
        public event Action<RaycastHit, int> OnMouseClick;
        private Camera _camera;
        
        [SerializeField] private PCPlayerRefs localPlayer;
        
        [SerializeField] private LayerMask _layerMask = Constants.Layers.GroundMask | Constants.Layers.EntityMask;

        [SerializeField] private List<CursorWrapper> cursorWrappers;
        private readonly Dictionary<CursorType, CursorData> _cursors = new();
        
        private RaycastHit _currentHit;
        private bool _hasHit;
        
        private Entity _currentPlayerRefs;
        
        public override void OnNetworkSpawn()
        {
            if (IsOwner)
            {
                _camera = Camera.main;
                InputManager.instance.onMouseButton1.performed += OnMouseButton1_FireEvent;
                
                foreach (var cursorWrapper in cursorWrappers)
                {
                    _cursors.TryAdd(cursorWrapper.type, cursorWrapper.data);
                }
                
                UpdateCursor();
            }
        }

        public override void OnNetworkDespawn()
        {
            base.OnNetworkDespawn();
            
            if (IsOwner)
            {
                _camera = null;
                if (InputManager.IsInstanceAlive()) InputManager.instance.onMouseButton1.performed -= OnMouseButton1_FireEvent;
                
                CursorManager.Release();
            }
        }


        private void OnMouseButton1_FireEvent(InputAction.CallbackContext _)
        {
            if (!_hasHit) return;
            
            OnMouseClick?.Invoke(_currentHit, _currentHit.collider.gameObject.layer);
        }

        private void Update()
        {
            if (!IsOwner) return;

            _hasHit = Utilities.GetMouseWorldHit(_camera, _layerMask, out RaycastHit hitInfo);
            if (!_hasHit)
            {
                OnNoPlayerHit();
                return;
            }
            
            _currentHit = hitInfo;

            CheckForPlayer();
        }

        private void CheckForPlayer()
        {
            if (_currentHit.collider.gameObject.layer != Constants.Layers.EntityIndex)
            {
                OnNoPlayerHit();
                return;
            }
                
            var obj = _currentHit.collider.gameObject;

            if (!obj.TryGetComponent<Entity>(out var entity))
            {
                OnNoPlayerHit();
                return;
            }

            OnPlayerHit(entity);
        }
        
        private void OnPlayerHit(Entity entity)
        {
            if(_currentPlayerRefs != null) _currentPlayerRefs.OnUnhover();
            
            _currentPlayerRefs = entity;

            if (_currentPlayerRefs.IsHovered) return;
            
            UpdateCursor();
            
            _currentPlayerRefs.OnHover();
        }
        
        private void OnNoPlayerHit()
        {
            if (!_currentPlayerRefs) return;
            
            _currentPlayerRefs.OnUnhover();
            _currentPlayerRefs = null;
            
            UpdateCursor();
        }
        
        #region Cursors
        
        // Aled c'est nul
        // TODO: refactor this
        private CursorType GetCurrentCursorType()
        {
            if (IsDefaultCursor()) return CursorType.Default;
            if (IsSpellCursor()) return CursorType.Spell;
            if (IsForbiddenCursor()) return CursorType.Forbidden;
            
            return CursorType.Attack;
        }
        
        private bool IsDefaultCursor()
        {
            return !_hasHit || _currentPlayerRefs == null || _currentPlayerRefs.TeamIndex == UserInstance.Me.Team;
        }
        
        private bool IsSpellCursor()
        {
            return localPlayer.SpellCastController.IsCasting;
        }
        
        private bool IsForbiddenCursor()
        {
            return localPlayer.InCastController.IsCasting || localPlayer.Channeling.IsChanneling;
        }

        private void UpdateCursor()
        {
            CursorManager.Release();
            
            var type = GetCurrentCursorType();
            var cursor = _cursors[type];
            
            CursorManager.Request(cursor.texture, cursor.hotspot, CursorMode.Auto);
        }
        
        #endregion
    }
}