import { ApiEnum } from "@/apis";
import { ConnectRequest, ConnectResponse } from "@/apis/Connect";
import Button from "@/components/Button";
import useApi from "@useApi";
import { toggleNotification } from "@/stores/NotificationReducer";
import { RootState } from "@/stores/store";
import { LoadingState } from "@/types";
import { MessageOutlined } from "@ant-design/icons";
import { Wifi } from "@icon-park/react";
import { Tooltip } from "antd";
import React, { useEffect, useState } from "react";
import { useSelector } from "react-redux";
import { useDispatch } from "react-redux";
import LoadingWrapper from "@/components/LoadingWrapper";

const FootContent = () => {
  const dispatch = useDispatch();
  const api = useApi();
  const active = useSelector((state: RootState) => state.socket.active);
  const [loading, setLoading] = useState<LoadingState>(LoadingState.Done);
  const handleClick = async () => {
    if (!active) {
      setLoading(LoadingState.Loading);
      api.request<ConnectRequest, ConnectResponse>(ApiEnum.Connect, {});
    }
  };
  useEffect(() => {
    api.request<ConnectRequest, ConnectResponse>(ApiEnum.Connect, {});
  }, []);

  useEffect(() => {
    if (active) {
      setLoading(LoadingState.Done);
    }
  }, [active]);
  return (
    <div className='bg-white flex flex-row-reverse p-2'>
      <Button onClick={() => dispatch(toggleNotification())} variant='primary'>
        <MessageOutlined />
      </Button>
      <Button
        variant='primary'
        onClick={() => handleClick()}
        disabled={loading === LoadingState.Loading}>
        <Tooltip title={active ? "已连接" : "未连接, 点击尝试连接"}>
          <LoadingWrapper loading={loading}>
            <Wifi theme='filled' size='18' fill={active ? "#1890ff" : "#ff4d4f"} />
          </LoadingWrapper>
        </Tooltip>
      </Button>
    </div>
  );
};

export default FootContent;
