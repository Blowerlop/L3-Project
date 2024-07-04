using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Reflection.Emit;
using Sirenix.OdinInspector;
using Sirenix.Serialization;
using UnityEngine;
using UnityEngine.Events;
using OpCodes = System.Reflection.Emit.OpCodes;

namespace Project
{
    public enum EInitialization
    {
        Awake,
        Start,
        OnEnable,
        OnDisable,
        OnDestroy
    }
    
    public enum EDecommissioning
    {
        OnDisable,
        OnDestroy,
        Fire
    }
    
    public class EventSubscriber : SerializedMonoBehaviour
    {
        public const BindingFlags BINDING_FLAGS_MONOBEHAVIOUR = BindingFlags.Instance | BindingFlags.Public;
        public const BindingFlags BINDING_FLAGS_TYPE = BindingFlags.Static | BindingFlags.Public;

        [Title("Target")] 
        [SerializeField] private bool _searchByInstance = true;
        [SerializeField, ShowIf(nameof(_searchByInstance))] private MonoBehaviour _targetMonoBehaviour;
        [OdinSerialize, HideIf(nameof(_searchByInstance))] private Type _targetType;
        [SerializeField, HideIf(nameof(_searchByInstance))] private bool _isSingleton = false;
        [OdinSerialize, ValueDropdown(nameof(GetEventsName))] private string _memberInfoName;
        [NonSerialized] private MemberInfo _memberInfo;
        
        [Title("Execution Timing")]
        [SerializeField] private EInitialization _subscribeExecutionTiming;
        [SerializeField] private EDecommissioning _unsubscribeExecutionTiming;
        
        [Title("Callback")]
        [SerializeField] public UnityEvent _callback;
        [OdinSerialize, HideInInspector] private Delegate _delegate;


        private void Awake()
        {
            if (_subscribeExecutionTiming == EInitialization.Awake) Subscribe();
        }
        
        private void Start()
        {
            if (_subscribeExecutionTiming == EInitialization.Start) Subscribe();
        }
        
        private void OnEnable()
        {
            if (_subscribeExecutionTiming == EInitialization.OnEnable) Subscribe();
        }
        
        private void OnDisable()
        {
            if (_unsubscribeExecutionTiming == EDecommissioning.OnDisable) Unsubscribe();
        }
        
        private void OnDestroy()
        {
            if (_unsubscribeExecutionTiming == EDecommissioning.OnDestroy) Unsubscribe();
        }


        private void Subscribe()
        {
            if (string.IsNullOrEmpty(_memberInfoName)) return;
            if (GetTargetObject() == null) return;
            
            _memberInfo = GetTargetType().GetEvent(_memberInfoName, GetBindingFlags());
            
            if (_memberInfo == null) throw new Exception($"Event or Field {_memberInfoName} not found in {GetTargetType()}");

            CreateDelegate();

            if (_memberInfo is EventInfo eventInfo)
            {
                eventInfo.AddEventHandler(GetTargetObject(), _delegate);
            }
        }
        
        private void Unsubscribe()
        {
            if (_memberInfo == null) throw new Exception($"Event or Field {_memberInfoName} not found in {GetTargetType()}");
            if (GetTargetObject() == null) return;
            
            if (_memberInfo is EventInfo eventInfo) eventInfo.RemoveEventHandler(GetTargetObject(), _delegate);
        }


        #region  Editor

        private IEnumerable<string> GetEventsName()
        {
            Type targetType = GetTargetType();

            var eventFields = targetType.GetEvents(GetBindingFlags()).Cast<MemberInfo>();
            
            return eventFields.Select(x => x.Name);
        }

        private Type GetTargetType()
        {
            if (_searchByInstance) return _targetMonoBehaviour.GetType();
            return _targetType;
        }
        
        private object GetTargetObject()
        {
            if (_searchByInstance) return _targetMonoBehaviour;
            if (_isSingleton) return _targetType.GetProperties(BindingFlags.Instance | BindingFlags.Public | BindingFlags.Static).First(x => x.PropertyType == GetTargetType()).GetValue(null);
            return _targetType;
        }
        
        private BindingFlags GetBindingFlags()
        {
            if (_searchByInstance || _isSingleton) return BINDING_FLAGS_MONOBEHAVIOUR;
            return BINDING_FLAGS_TYPE;
        }
        
        private void CreateDelegate()
        {
            if (string.IsNullOrEmpty(_memberInfoName)) return;
            
            if (_memberInfo == null) throw new Exception($"Event or Field {_memberInfoName} not found in {GetTargetType()}");
            

            if (_memberInfo is EventInfo eventInfo) 
            {
                Type tDelegate = eventInfo.EventHandlerType;

                // Add the EventSubscriber type as the first parameter type
                Type[] parameterTypes = GetDelegateParameterTypes(tDelegate);
                Type[] parameterTypesWithThis = new Type[parameterTypes.Length + 1];
                parameterTypesWithThis[0] = typeof(EventSubscriber);
                Array.Copy(parameterTypes, 0, parameterTypesWithThis, 1, parameterTypes.Length);

                DynamicMethod handler = new DynamicMethod("", null, parameterTypesWithThis);
                ILGenerator ilgen = handler.GetILGenerator();

                MethodInfo invokeCallbackMethod = typeof(EventSubscriber).GetMethod(nameof(InvokeCallback));

                // Load the EventSubscriber instance onto the stack
                ilgen.Emit(OpCodes.Ldarg_0);

                // Call the function
                ilgen.Emit(OpCodes.Call, invokeCallbackMethod);

                ilgen.Emit(OpCodes.Ret);

                // Create a delegate that takes the EventSubscriber instance as the first argument
                _delegate = handler.CreateDelegate(tDelegate, this);
            }
        }

        #endregion
        
        private Type[] GetDelegateParameterTypes(Type d)
        {
            if (d.BaseType != typeof(MulticastDelegate))
                throw new ArgumentException("Not a delegate.", nameof(d));

            MethodInfo invoke = d.GetMethod("Invoke");
            if (invoke == null)
                throw new ArgumentException("Not a delegate.", nameof(d));

            ParameterInfo[] parameters = invoke.GetParameters();
            Type[] typeParameters = new Type[parameters.Length];
            for (int i = 0; i < parameters.Length; i++)
            {
                typeParameters[i] = parameters[i].ParameterType;
            }
            return typeParameters;
        }

        private Type GetDelegateReturnType(Type d)
        {
            if (d.BaseType != typeof(MulticastDelegate))
                throw new ArgumentException("Not a delegate.", nameof(d));

            MethodInfo invoke = d.GetMethod("Invoke");
            if (invoke == null)
                throw new ArgumentException("Not a delegate.", nameof(d));

            return invoke.ReturnType;
        }

        public void InvokeCallback()
        {
            _callback.Invoke();
            if (_unsubscribeExecutionTiming == EDecommissioning.Fire) Unsubscribe();
        }
    }
}
