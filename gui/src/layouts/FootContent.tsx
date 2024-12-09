import { ApiEnum } from "@/apis";
import { ConnectRequest, ConnectResponse } from "@/apis/Connect";
import Button from "@/components/Button";
import useApi from "@useApi";
import { toggleNotification } from "@/stores/NotificationReducer";
import { RootState } from "@/stores/store";
import { CloseWifi, MessageUnread, Wifi } from "@icon-park/react";
import { Spin, Tooltip } from "antd";
import React, { useCallback, useEffect } from "react";
import { useSelector, useDispatch } from "react-redux";

const FootContent = () => {
  const dispatch = useDispatch();
  const api = useApi();
  const active = useSelector((state: RootState) => state.socket.active);
  const connecting = useSelector((state: RootState) => state.socket.connecting);
  const handleClick = useCallback(() => {
    if (!active) {
      api.request<ConnectRequest, ConnectResponse>(ApiEnum.Connect, {});
    }
  }, [active]);
  useEffect(() => {
    api.request<ConnectRequest, ConnectResponse>(ApiEnum.Connect, {});
  }, []);
  return (
    <div className='bg-white flex-row-reverse p-2 fixed bottom-0 z-50 flex w-full right-0'>
      <Button onClick={() => dispatch(toggleNotification())} variant='primary'>
        <MessageUnread theme='filled' size='24' fill='#333' strokeLinecap='butt' />
      </Button>
      <Button variant='text' onClick={() => handleClick()} disabled={connecting}>
        {connecting ? (
          <Spin size='small' />
        ) : (
          <Tooltip title={active ? "已连接" : "未连接, 点击尝试连接"}>
            {active ? (
              <Wifi theme='filled' size='24' fill='#333' strokeLinecap='butt' />
            ) : (
              <CloseWifi theme='filled' size='24' fill='#333' strokeLinecap='butt' />
            )}
          </Tooltip>
        )}
      </Button>
    </div>
  );
};

export default FootContent;
