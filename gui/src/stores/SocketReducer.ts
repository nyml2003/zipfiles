import { createSlice } from "@reduxjs/toolkit";

export interface SocketState {
  active: boolean;
  connecting: boolean;
}

const initialState: SocketState = {
  active: false,
  connecting: false
};

const SocketReducer = createSlice({
  name: "socket",
  initialState,
  reducers: {
    enableSocket(state : SocketState) {
      state.active = true;
      state.connecting = false;
    },
    disableSocket(state : SocketState) {
      state.active = false;
      state.connecting = false;
    },
    connectSocket(state: SocketState) {
      state.connecting = true;
    }
  }
});

export const {
  enableSocket,
  disableSocket,
  connectSocket
} = SocketReducer.actions;

export default SocketReducer.reducer;
