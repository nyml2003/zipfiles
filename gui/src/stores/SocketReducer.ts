import { createSlice } from "@reduxjs/toolkit";

export interface SocketState {
  active: boolean;
}

const initialState: SocketState = {
  active: false,
};

const SocketReducer = createSlice({
  name: "socket",
  initialState,
  reducers: {
    enableSocket(state : SocketState) {
      state.active = true;
    },
    disableSocket(state : SocketState) {
      state.active = false;
    },
  },
});

export const {
  enableSocket,
  disableSocket,
} = SocketReducer.actions;

export default SocketReducer.reducer;
