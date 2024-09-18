import { Icon } from '@douyinfe/semi-ui'
import IconGameSvg from './handheld.svg'
import { IconProps } from '@douyinfe/semi-icons'
import React from 'react'
const IconGame = (
  props: Omit<Omit<IconProps, 'svg' | 'type'>, 'ref'> & React.RefAttributes<HTMLSpanElement>,
) => {
  return (
    <Icon
      svg={<IconGameSvg />}
      className='semi-icon semi-icon-default'
      aria-label='game'
      {...props}
    />
  )
}

export default IconGame
