using System;
using Sirenix.OdinInspector;
using UnityEngine;
using UnityEngine.EventSystems;

namespace Project
{
    public enum HandlerType
    {
        TopRight,
        Right,
        BottomRight,
        Bottom,
        BottomLeft,
        Left,
        TopLeft,
        Top
    }
    
    public class ResizableWindow : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler, IDragHandler, IEndDragHandler
    {
        [Title("Target")]
        private Canvas _canvas;
        [SerializeField] private bool _selfTarget = false;
        [SerializeField, HideIf("_selfTarget")] private RectTransform _target;

        [Title("Resizable Parameters")]
        [SerializeField] private HandlerType _handler;
        [SerializeField] private bool _clampSize = false;
        [SerializeField, ShowIf("_clampSize")] private bool _clampMinimum = true;
        [SerializeField, ShowIf("@_clampSize && _clampMinimum")] private Vector2 _minimumDimensions = new Vector2(50, 50);
        [SerializeField, ShowIf("_clampSize")] private bool _clampMaximum = true;
        [SerializeField, ShowIf("@_clampSize && _clampMaximum")] private Vector2 _maximumDimensions = new Vector2(800, 800);

        [Title("Optional Parameters")] 
        [SerializeField] private bool _autoUpdateCursorVisual;
        [SerializeField, ShowIf("_autoUpdateCursorVisual")] private Texture2D _cursorTexture2D;
        [InfoBox("The offset from the top left of the texture to use as the target point. This must be in the bounds of the cursor")]
        [SerializeField, ShowIf("_autoUpdateCursorVisual")] private Vector2 _hotSpot;
        [SerializeField, ShowIf("_autoUpdateCursorVisual")] private CursorMode _cursorMode;

        [Title("Hacks :(")]
        private RectTransform _rectTransform;
        public Vector2 _defaultAnchorPosition;
        public Vector2 _defaultSizeDelta;

        [Title("Events")]
        // [SerializeField] private UnityEvent _onPointerEnterEvent = new UnityEvent();
        // [SerializeField] private UnityEvent _onPointerExitEvent = new UnityEvent();
        // [SerializeField] private Event _onPointerExitEventergergerg = new Event(nameof(_onPointerExitEventergergerg));
        private Action _onPointerEnterEvent;
        private Action _onPointerExitEvent;


        [ClearOnReload] private static bool _dragged = false;
        [ClearOnReload] private static ResizableWindow _draggedUser = null;
        
        
        private void Awake()
        {
            if (_selfTarget) _target = GetComponent<RectTransform>();
            
            _canvas = GetComponentInParent<Canvas>();
            _rectTransform = GetComponent<RectTransform>();
        }

        private void Start()
        {
            // _onPointerExitEventergergerg.Init();
            // Invoke("set_name", 0.0f);

            _defaultAnchorPosition = GetComponent<RectTransform>().anchoredPosition;
            _defaultSizeDelta = GetComponent<RectTransform>().sizeDelta;
            
            float originalWidth = _target.rect.width;
            float originalHeight = _target.rect.height;
            _minimumDimensions = new Vector2 (0.1f * originalWidth, 0.1f * originalHeight);
            _maximumDimensions = new Vector2 (10f * originalWidth, 10f * originalHeight);
        }

        private void OnEnable()
        {
            if (_autoUpdateCursorVisual)
            {
                _onPointerEnterEvent += UpdateCursorVisualOnPointerEnter;
                _onPointerExitEvent += CursorManager.Release;
            }
        }

        private void OnDisable()
        {
            if (_autoUpdateCursorVisual)
            {
                _onPointerEnterEvent -= UpdateCursorVisualOnPointerEnter;
                _onPointerExitEvent -= CursorManager.Release;
            }
        }

        public void OnPointerEnter(PointerEventData eventData)
        {
            if (_dragged && _draggedUser != this) return;
            
            _rectTransform.sizeDelta = _defaultSizeDelta;
            _rectTransform.anchoredPosition = _defaultAnchorPosition;

            _onPointerEnterEvent?.Invoke();
        }

        public void OnPointerExit(PointerEventData eventData)
        {
            if (_dragged && _draggedUser != this) return;
            
            _rectTransform.sizeDelta = _defaultSizeDelta;
            _rectTransform.anchoredPosition = _defaultAnchorPosition;
            
            _onPointerExitEvent?.Invoke();
        }
        
        public void OnDrag(PointerEventData eventData)
        {
            _draggedUser = this;
            _dragged = true;
            
            _rectTransform.sizeDelta = _defaultSizeDelta;
            _rectTransform.anchoredPosition = _defaultAnchorPosition;
            
            _rectTransform.sizeDelta *= 100;
            _rectTransform.anchoredPosition *= 50;
            
            RectTransform.Edge? horizontalEdge = null; 
            RectTransform.Edge? verticalEdge = null;
            
            switch (_handler)
            {
                case HandlerType.TopRight:
                    horizontalEdge = RectTransform.Edge.Left;
                    verticalEdge = RectTransform.Edge.Bottom;
                    break;
                case HandlerType.Right:
                    horizontalEdge = RectTransform.Edge.Left;
                    break;
                case HandlerType.BottomRight:
                    horizontalEdge = RectTransform.Edge.Left;
                    verticalEdge = RectTransform.Edge.Top;
                    break;
                case HandlerType.Bottom:
                    verticalEdge = RectTransform.Edge.Top;
                    break;
                case HandlerType.BottomLeft:
                    horizontalEdge = RectTransform.Edge.Right;
                    verticalEdge = RectTransform.Edge.Top;
                    break;
                case HandlerType.Left:
                    horizontalEdge = RectTransform.Edge.Right;
                    break;
                case HandlerType.TopLeft:
                    horizontalEdge = RectTransform.Edge.Right;
                    verticalEdge = RectTransform.Edge.Bottom;
                    break;
                case HandlerType.Top:
                    verticalEdge = RectTransform.Edge.Bottom;
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
            
            ResizeWindow();
            
            
            
            
            void ResizeWindow()
            {
                Vector2 sizeDelta;
                Vector2 scaledEventDataDelta = (eventData.delta / _canvas.scaleFactor);
                
                
                if (horizontalEdge != null)
                {
                    sizeDelta = _target.sizeDelta;
                    float newWidth;
                    float deltaPosX;
                    
                    if (horizontalEdge == RectTransform.Edge.Right)
                    {
                        // if (_clampSize)
                        // {
                        //     if (_clampMinimum)
                        //     {
                        //         newWidth = Mathf.Clamp(sizeDelta.x - scaledEventDataDelta.x, _minimumDimensions.x, Mathf.Infinity);
                        //     }
                        //     else if (_clampMaximum)
                        //     {
                        //         newWidth = Mathf.Clamp(sizeDelta.x - scaledEventDataDelta.x, Mathf.Infinity, _maximumDimensions.x);
                        //     }
                        //     else return;
                        // }
                        // else
                        // {
                        //     newWidth = sizeDelta.x - scaledEventDataDelta.x;
                        // }
                        
                        newWidth = sizeDelta.x - scaledEventDataDelta.x;
                        
                        deltaPosX = -(newWidth - sizeDelta.x) * _target.pivot.x;
                        
                        _target.sizeDelta = new Vector2(newWidth, _target.sizeDelta.y);
                        _target.anchoredPosition += new Vector2(deltaPosX, 0);
                    }
                    else
                    {
                        if (_clampSize)
                        {
                            if (_clampMinimum)
                            {
                                newWidth = Mathf.Clamp(sizeDelta.x + scaledEventDataDelta.x,  _minimumDimensions.x, Mathf.Infinity);
                            }
                            else if (_clampMaximum)
                            {
                                newWidth = Mathf.Clamp(sizeDelta.x + scaledEventDataDelta.x, Mathf.Infinity, _maximumDimensions.x);
                            }
                            else return;
                        }
                        else
                        {
                            newWidth = sizeDelta.x + scaledEventDataDelta.x;
                        }
                        
                        deltaPosX = (newWidth - sizeDelta.x) * _target.pivot.x;

                        _target.sizeDelta = new Vector2(newWidth, _target.sizeDelta.y);
                        _target.anchoredPosition += new Vector2(deltaPosX, 0);
                    }
                }
                if (verticalEdge != null)
                {
                    sizeDelta = _target.sizeDelta;
                    float newHeight;
                    float deltaPosY;
                    
                    if (verticalEdge == RectTransform.Edge.Top)
                    {
                        newHeight = sizeDelta.y - scaledEventDataDelta.y;
                        deltaPosY = -(newHeight - sizeDelta.y) * _target.pivot.y;
                
                        _target.sizeDelta = new Vector2(_target.sizeDelta.x, newHeight);
                        _target.anchoredPosition += new Vector2(0, deltaPosY);
                    }
                    else
                    {
                        newHeight = sizeDelta.y + scaledEventDataDelta.y;
                        deltaPosY = (newHeight - sizeDelta.y) * _target.pivot.y;
                
                        _target.sizeDelta = new Vector2(_target.sizeDelta.x, newHeight);
                        _target.anchoredPosition += new Vector2(0, deltaPosY);
                    }
                }
            }
        }
        
        public void OnEndDrag(PointerEventData eventData)
        {
            _rectTransform.sizeDelta = _defaultSizeDelta;
            _rectTransform.anchoredPosition = _defaultAnchorPosition;
            
            _draggedUser = null;
            _dragged = false;
        }
        
        
        private void UpdateCursorVisualOnPointerEnter()
        {
            CursorManager.Request(_cursorTexture2D, _hotSpot, _cursorMode);
        }
    }
}
