import { ApiEnum } from "@/apis";
import { ConnectRequest, ConnectResponse } from "@/apis/Connect";
import Button from "@/components/Button";
import useApi from "@/hooks/useApi";
import { toggleNotification } from "@/stores/NotificationReducer";
import { RootState } from "@/stores/store";
import {
  MessageOutlined,
} from "@ant-design/icons";
import { Wifi } from "@icon-park/react";
import React from "react";
import { useSelector } from "react-redux";
import { useDispatch } from "react-redux";

const FootContent = () => {
  const dispatch = useDispatch();
  const api = useApi();
  const active = useSelector((state: RootState) => state.socket.active);
  const handleClick = async () => {
    if (!active) {
      console.log("尝试连接");
      await api.request<ConnectRequest, ConnectResponse>(ApiEnum.Connect, {});
    }
  };
  return (
    <div className='bg-white flex flex-row-reverse p-2'>
      <Button onClick={() => dispatch(toggleNotification())} variant='primary'>
        <MessageOutlined />
      </Button>
      <Button variant='primary' onClick= {() => handleClick()}>
        <Wifi theme='filled' size='18' fill={active ? "#1890ff" : "#ccc"} />
      </Button>
    </div>
  );
};

export default FootContent;
