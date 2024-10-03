import { ActionTypes } from './types';

interface IncrementAction {
  type: ActionTypes.INCREMENT;
}

interface DecrementAction {
  type: ActionTypes.DECREMENT;
}

export type CounterAction = IncrementAction | DecrementAction;

export const increment = (): IncrementAction => ({
  type: ActionTypes.INCREMENT,
});

export const decrement = (): DecrementAction => ({
  type: ActionTypes.DECREMENT,
});
