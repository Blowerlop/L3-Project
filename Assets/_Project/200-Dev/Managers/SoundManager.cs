using System.Collections;
using System.Collections.Generic;
using _Project._200_Dev.User;
using _Project._200_Dev.Utilities.Objects;
using FMOD;
using FMOD.Studio;
using FMODUnity;
using Sirenix.Utilities;
using UnityEngine;
using Debug = UnityEngine.Debug;
using STOP_MODE = FMOD.Studio.STOP_MODE;

namespace _Project._200_Dev.Managers
{
    public class SoundInstance
    {
        public string EventName;
        public SoundManager.EventType EventType;
        public EventInstance EventInstance;
        public SoundInstance(string eventName, SoundManager.EventType eventType, EventInstance eventInstance)
        {
            EventName = eventName;
            EventType = eventType;
            EventInstance = eventInstance;
        }
    }

    public class PlayerAction
    {
        public UserInstance Player;
        public int Priority;
        public float LifeTime;
        public float CreatedTime;
        public string ActionName;
        public bool CutLowerPriority;
    }
    
    public class SoundManager : MonoSingleton<SoundManager>
    {
        private List<string> _eventsPath;

        private readonly Dictionary<string, SoundInstance> _staticSoundList = new();
        private readonly Dictionary<string, SoundInstance> _globalSoundList = new();
        private readonly Dictionary<string, SoundInstance> _snapshotList = new();
        private readonly Dictionary<string, Bus> _busList = new();
        private Dictionary<int, PlayerAction> _actionPool;

        private IEnumerator _poolCoroutine;

        public enum EventType
        {
            Music,
            Ambiance,
            PlayerAction,
            SFX,
            Spell,
            UI,
            Snapshot,
        }
        

        void Start()
        {
            _poolCoroutine = ConsumePoolCoroutine();

            StartCoroutine(_poolCoroutine);
        }

        #region Sound
        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="eventName"></param>
        /// <param name="eventAlias"></param>
        /// <param name="type"></param>
        public void PlayGlobalSound(string eventName, string eventAlias, EventType type)
        {
            GetPath(ref eventName, type);
            
            if (CheckErrorMessage(RuntimeManager.StudioSystem.getEvent(eventName, out var eventInfo), eventName))
                return;

            if (_globalSoundList.TryGetValue(eventAlias, out var value))
            {
                value.EventInstance.getPlaybackState(out var state);
                if(state == PLAYBACK_STATE.PLAYING)
                    value.EventInstance.stop(STOP_MODE.ALLOWFADEOUT);
                _globalSoundList.Remove(eventAlias);
            }

            eventInfo.createInstance(out var eventInstance);
            
            if(CheckErrorMessage(eventInstance.start()))
                return;
            
            _globalSoundList.Add(eventAlias, new SoundInstance(eventName, type, eventInstance));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="eventAlias"></param>
        public void StopGlobalSound(string eventAlias)
        {
            if (_staticSoundList.TryGetValue(eventAlias, out var value))
            {
                value.EventInstance.getPlaybackState(out var state);
                if(state == PLAYBACK_STATE.PLAYING)
                    value.EventInstance.stop(STOP_MODE.ALLOWFADEOUT);
                _staticSoundList.Remove(eventAlias);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="eventName"></param>
        /// <param name="eventAlias"></param>
        /// <param name="position"></param>
        /// <param name="type"></param>
        public void PlayStaticSound(string eventName, string eventAlias, Vector3 position, EventType type)
        {
            GetPath(ref eventName, type);
            
            if (CheckErrorMessage(RuntimeManager.StudioSystem.getEvent(eventName, out var eventInfo), eventName))
                return;

            if (_staticSoundList.TryGetValue(eventAlias, out var value))
            {
                value.EventInstance.getPlaybackState(out var state);
                if(state == PLAYBACK_STATE.PLAYING)
                    value.EventInstance.stop(STOP_MODE.ALLOWFADEOUT);
                _staticSoundList.Remove(eventAlias);
            }

            eventInfo.createInstance(out var eventInstance);
            
            eventInstance.set3DAttributes(position.To3DAttributes());
            
            if(CheckErrorMessage(eventInstance.start()))
                return;
            
            _staticSoundList.Add(eventAlias, new SoundInstance(eventName, type, eventInstance));
        }
        
        public void PlayStaticSound(string eventName, string eventAlias, GameObject target, EventType type)
        {
            GetPath(ref eventName, type);
            
            if (CheckErrorMessage(RuntimeManager.StudioSystem.getEvent(eventName, out var eventInfo), eventName))
                return;

            if (_staticSoundList.TryGetValue(eventAlias, out var value))
            {
                value.EventInstance.getPlaybackState(out var state);
                if(state == PLAYBACK_STATE.PLAYING)
                    value.EventInstance.stop(STOP_MODE.ALLOWFADEOUT);
                _staticSoundList.Remove(eventAlias);
            }
            
            eventInfo.createInstance(out var eventInstance);

            RuntimeManager.AttachInstanceToGameObject(eventInstance, target == null ? transform : target.transform);
            
            if(CheckErrorMessage(eventInstance.start()))
                return;
            
            _staticSoundList.Add(eventAlias, new SoundInstance(eventName, type, eventInstance));
        }
        
        public void PlaySingleSound(string eventName, GameObject target, EventType type)
        {
            if (eventName == "") return;
            GetPath(ref eventName, type);

            if (CheckErrorMessage(RuntimeManager.StudioSystem.getEvent(eventName, out var eventInfo), eventName)) 
                return;
            
            eventInfo.createInstance(out var eventInstance);
            
            RuntimeManager.AttachInstanceToGameObject(eventInstance, target.transform);

            CheckErrorMessage(eventInstance.start());
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="eventAlias"></param>
        public void StopStaticSound(string eventAlias)
        {
            if (_staticSoundList.TryGetValue(eventAlias, out var value))
            {
                value.EventInstance.getPlaybackState(out var state);
                if(state == PLAYBACK_STATE.PLAYING)
                    value.EventInstance.stop(STOP_MODE.ALLOWFADEOUT);
                _staticSoundList.Remove(eventAlias);
            }
        }
        
        public void TriggerSustain(string eventAlias)
        {
            if (_staticSoundList.TryGetValue(eventAlias, out var value))
            {
                value.EventInstance.getPlaybackState(out var state);
                if(state is PLAYBACK_STATE.PLAYING or PLAYBACK_STATE.SUSTAINING)
                    value.EventInstance.keyOff();
                _staticSoundList.Remove(eventAlias);
            }
        }

        public void AddPlayerActionSound(string actionName, UserInstance player)
        {
            //TODO
        }

        public void ClearPlayerActionPool(UserInstance player)
        {
            //TODO
        }

        public void ClearAllPlayerActionPool()
        {
            //TODO
        }

        public IEnumerator ConsumePoolCoroutine()
        {
            yield return null; //TODO
        }

        #endregion

        #region Bus

        public void SetBusVolume(float volume, string busKey)
        {
            if (_busList.TryGetValue(busKey, out var busVal))
            {
                busVal.setVolume(volume);
                return;
            }
            
            var bus = RuntimeManager.GetBus("bus:/" + busKey);
            bus.setVolume(volume);
            _busList.Add(busKey, bus);
        }

        #endregion

        #region Parameters

        public void SetGlobalParameterValue(string parameterName, int parameterValue)
        {
            CheckErrorMessage(RuntimeManager.StudioSystem.setParameterByName(parameterName, parameterValue),
                parameterName);
        }
        
        public void SetGlobalParameterValue(string parameterName, string parameterValue)
        {
            CheckErrorMessage(RuntimeManager.StudioSystem.setParameterByNameWithLabel(parameterName, parameterValue),
                parameterName);
        }
        
        public void SetGlobalSoundParameterValue(string parameterName, int parameterValue, string eventAlias)
        {
            if(_globalSoundList.TryGetValue(eventAlias, out var eventInstance))
            {
                CheckErrorMessage(eventInstance.EventInstance.setParameterByName(parameterName, parameterValue),
                    parameterName);
            }
        }
        
        public void SetGlobalSoundParameterValue(string parameterName, string parameterValue, string eventAlias)
        {
            if(_globalSoundList.TryGetValue(eventAlias, out var eventInstance))
            {
                CheckErrorMessage(
                    eventInstance.EventInstance.setParameterByNameWithLabel(parameterName, parameterValue),
                    parameterName);
            }
        }
        
        public void SetStaticSoundParameterValue(string parameterName, int parameterValue, string eventAlias)
        {
            if(_staticSoundList.TryGetValue(eventAlias, out var eventInstance))
            {
                CheckErrorMessage(eventInstance.EventInstance.setParameterByName(parameterName, parameterValue),
                    parameterName);
            }
        }
        
        public void SetStaticSoundParameterValue(string parameterName, string parameterValue, string eventAlias)
        {
            if(_staticSoundList.TryGetValue(eventAlias, out var eventInstance))
            {
                CheckErrorMessage(
                    eventInstance.EventInstance.setParameterByNameWithLabel(parameterName, parameterValue),
                    parameterName);
            }
        }

        #endregion
        
        #region Utils

        public void TriggerSnapshot(string snapshotName, string snapshotAlias)
        {
            if (CheckErrorMessage(RuntimeManager.StudioSystem.getEvent("snapshot:/" + snapshotName, out var eventInfo), snapshotName))
                return;

            if (_snapshotList.TryGetValue(snapshotAlias, out var value))
            {
                value.EventInstance.getPlaybackState(out var state);
                if(state == PLAYBACK_STATE.PLAYING)
                    value.EventInstance.stop(STOP_MODE.ALLOWFADEOUT);
                _snapshotList.Remove(snapshotAlias);
            }
            
            eventInfo.createInstance(out var eventInstance);
            
            if(CheckErrorMessage(eventInstance.start()))
                return;

            _snapshotList.Add(snapshotAlias,
                new SoundInstance("snapshot:/" + snapshotName, EventType.Snapshot, eventInstance));
        }
        
        public void StopSnapshot(string snapshotAlias)
        {
            if (_snapshotList.TryGetValue(snapshotAlias, out var value))
            {
                value.EventInstance.getPlaybackState(out var state);
                if(state == PLAYBACK_STATE.PLAYING)
                    value.EventInstance.stop(STOP_MODE.ALLOWFADEOUT);
                _snapshotList.Remove(snapshotAlias);
                return;
            }
            
            Debug.LogWarning("Fmod: Event: " + snapshotAlias + " does not exist prévenir Guillaume");
        }
        
        public List<string> GetAllEvent()
        {
            if (_eventsPath != null) return _eventsPath;

            _eventsPath = new List<string>();
            RuntimeManager.StudioSystem.getBankList(out Bank[] loadedBanks);

            foreach (var bank in loadedBanks)
            {
                bank.getEventList(out var events);
                events.ForEach(x => x.getPath(out var path));
                foreach (var eventRef in events)
                {
                    eventRef.getPath(out var path);
                    _eventsPath.Add(path);
                }
            }

            return _eventsPath;
        }

        private void GetPath(ref string eventName, EventType type)
        {
            switch (type)
            {
                case EventType.Music:
                    eventName = "event:/Music/" + eventName;
                    break;
                case EventType.Ambiance:
                    eventName = "event:/Ambience/" + eventName;
                    break;
                case EventType.SFX:
                    eventName = "event:/SFX/" + eventName;
                    break;
                case EventType.Spell:
                    eventName = "event:/SFX/Spell/" + eventName;
                    break;
                case EventType.UI:
                    eventName = "event:/SFX/UI/" + eventName;
                    break;
                default:
                    eventName = "event:/" + eventName;
                    break;
            }
        }

        private bool CheckErrorMessage(RESULT result, string info = null)
        {
            switch (result)
            {
                case RESULT.OK:
                    return false;
                case RESULT.ERR_EVENT_NOTFOUND:
                case RESULT.ERR_FILE_NOTFOUND:
                case RESULT.ERR_INVALID_HANDLE:
                    Debug.LogWarning("Fmod: Event: " + info + " does not exist prévenir Guillaume");
                    return false;
                default:
                    Debug.LogError("Fmod: " + result + ", " + info + " prévenir Guillaume");
                    return true;
            }
        } 

        #endregion
    }
}
